// existing_user.cpp
#include "existing_user.h"
#include "new_user.h"
#include "forgot_password.h"
#include "CustomHash.h"
#include "Argon2Hash.h"
#include "otp.h"
#include "PasswordManager.h"

#include <aws/core/Aws.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <aws/dynamodb/model/UpdateItemRequest.h>
#include <aws/dynamodb/model/AttributeValueUpdate.h>
#include <aws/dynamodb/model/PutItemRequest.h>          
#include <aws/dynamodb/model/AttributeValue.h>          
#include <aws/email/SESClient.h>
#include <aws/email/model/SendEmailRequest.h>
#include <aws/email/model/Destination.h>
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>

bool handleLoginFailureOptions(bool& exitProgram);
void sendAccountLockEmail(const std::string& user_email, int lockout_duration_minutes);

void loginExistingUser() {
    const int max_attempts = 3;
    const int lockout_duration_minutes = 5;
    const int attempt_window_seconds = 3600;

    int attempts = 0;
    bool loggedIn = false;
    bool exitProgram = false;

    // Initialize AWS SDK
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        Aws::Client::ClientConfiguration clientConfig;
        Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);
        Aws::SES::SESClient sesClient(clientConfig); 

        while (!exitProgram && attempts < max_attempts) {
            std::string user_id;
            std::string password;

            std::cout << "Enter user_id: ";
            std::getline(std::cin, user_id);

            std::cout << "Enter password: ";
            std::getline(std::cin, password);

            try {
                Aws::DynamoDB::Model::GetItemRequest request;
                request.SetTableName("SLS_Table");
                request.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

                auto outcome = dynamoClient.GetItem(request);

                if (outcome.IsSuccess()) {
                    const auto& item = outcome.GetResult().GetItem();
                    if (item.empty()) {
                        std::cerr << "Error: user_id does not exist." << std::endl;
                        attempts++;
                        if (!handleLoginFailureOptions(exitProgram)) {
                            break;
                        }
                        continue;
                    }

                    long long currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                    long long lockoutUntil = 0;
                    if (item.find("LockoutUntil") != item.end()) {
                        lockoutUntil = std::stoll(item.at("LockoutUntil").GetN());
                    }

                    if (currentTime < lockoutUntil) {
                        std::cout << "Your account is locked until "
                                  << std::ctime(reinterpret_cast<const time_t*>(&lockoutUntil))
                                  << "Please try again later." << std::endl;
                        Aws::ShutdownAPI(options);
                        return;
                    }

                    int failedAttempts = 0;
                    long long lastAttemptTime = 0;
                    if (item.find("FailedAttempts") != item.end()) {
                        failedAttempts = std::stoi(item.at("FailedAttempts").GetN());
                    }
                    if (item.find("LastAttemptTime") != item.end()) {
                        lastAttemptTime = std::stoll(item.at("LastAttemptTime").GetN());
                    }

                    lastAttemptTime = currentTime;

                    if (item.find("salt") == item.end() ||
                        item.find("hashed_password") == item.end() ||
                        item.find("user_email") == item.end()) {
                        std::cerr << "Error: Incomplete user data in database." << std::endl;
                        attempts++;
                        if (!handleLoginFailureOptions(exitProgram)) {
                            break;
                        }
                        continue;
                    }

                    std::string salt_hex = item.at("salt").GetS();
                    std::string storedHashedPassword = item.at("hashed_password").GetS();
                    std::string user_email = item.at("user_email").GetS();
                    std::string hashing_algo = item.at("hashing_algo").GetS();

                    std::string hashedPassword;

                    if (hashing_algo == "scratch") {
                        std::string saltedPassword = salt_hex + password;
                        CustomHash hasher;
                        unsigned long long hashedPasswordValue = hasher.computeHash(saltedPassword);
                        hashedPassword = hasher.hashToHex(hashedPasswordValue);
                    } else if (hashing_algo == "argon2") {
                        std::vector<uint8_t> salt = Argon2Hash::fromHex(salt_hex);
                        hashedPassword = Argon2Hash::computeHash(password, salt);
                    } else {
                        std::cerr << "Unsupported hashing algorithm." << std::endl;
                        continue;
                    }

                    if (hashedPassword != storedHashedPassword) {
                        std::cout << "Incorrect password." << std::endl;
                        attempts++;
                        failedAttempts++;

                        if (failedAttempts >= max_attempts) {
                            if ((currentTime - lastAttemptTime) <= attempt_window_seconds) {
                                long long lockoutUntilTime = currentTime + (lockout_duration_minutes * 60);
                                Aws::DynamoDB::Model::UpdateItemRequest updateRequest;
                                updateRequest.SetTableName("SLS_Table");
                                updateRequest.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

                                updateRequest.AddAttributeUpdates("LockoutUntil", Aws::DynamoDB::Model::AttributeValueUpdate()
                                    .WithValue(Aws::DynamoDB::Model::AttributeValue().SetN(std::to_string(lockoutUntilTime)))
                                    .WithAction(Aws::DynamoDB::Model::AttributeAction::PUT));

                                updateRequest.AddAttributeUpdates("FailedAttempts", Aws::DynamoDB::Model::AttributeValueUpdate()
                                    .WithValue(Aws::DynamoDB::Model::AttributeValue().SetN("0"))
                                    .WithAction(Aws::DynamoDB::Model::AttributeAction::PUT));

                                auto updateOutcome = dynamoClient.UpdateItem(updateRequest);
                                if (!updateOutcome.IsSuccess()) {
                                    std::cerr << "Failed to update lockout status: "
                                              << updateOutcome.GetError().GetMessage() << std::endl;
                                } else {
                                    std::cout << "Account locked due to multiple failed login attempts. Please try again after "
                                              << lockout_duration_minutes << " minutes." << std::endl;

                                    sendAccountLockEmail(user_email, lockout_duration_minutes);
                                    break;
                                }
                                
                            } else {
                                failedAttempts = 1;
                            }
                        }

                        Aws::DynamoDB::Model::UpdateItemRequest updateRequest;
                        updateRequest.SetTableName("SLS_Table");
                        updateRequest.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

                        updateRequest.AddAttributeUpdates("FailedAttempts", Aws::DynamoDB::Model::AttributeValueUpdate()
                            .WithValue(Aws::DynamoDB::Model::AttributeValue().SetN(std::to_string(failedAttempts)))
                            .WithAction(Aws::DynamoDB::Model::AttributeAction::PUT));

                        updateRequest.AddAttributeUpdates("LastAttemptTime", Aws::DynamoDB::Model::AttributeValueUpdate()
                            .WithValue(Aws::DynamoDB::Model::AttributeValue().SetN(std::to_string(lastAttemptTime)))
                            .WithAction(Aws::DynamoDB::Model::AttributeAction::PUT));

                        auto updateOutcome = dynamoClient.UpdateItem(updateRequest);
                        if (!updateOutcome.IsSuccess()) {
                            std::cerr << "Failed to update login attempts: "
                                      << updateOutcome.GetError().GetMessage() << std::endl;
                        }

                        if (!handleLoginFailureOptions(exitProgram)) {
                            break;
                        }
                        continue;
                    } else {
                
                        Aws::DynamoDB::Model::UpdateItemRequest updateRequest;
                        updateRequest.SetTableName("SLS_Table");
                        updateRequest.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

                        updateRequest.AddAttributeUpdates("FailedAttempts", Aws::DynamoDB::Model::AttributeValueUpdate()
                            .WithValue(Aws::DynamoDB::Model::AttributeValue().SetN("0"))
                            .WithAction(Aws::DynamoDB::Model::AttributeAction::PUT));

                        auto updateOutcome = dynamoClient.UpdateItem(updateRequest);
                        if (!updateOutcome.IsSuccess()) {
                            std::cerr << "Failed to reset failed attempts: " << updateOutcome.GetError().GetMessage() << std::endl;
                        }

                        std::cout << "Password verified. An OTP has been sent to your registered email." << std::endl;

                        if (!validateOTP(user_email)) {
                            std::cout << "OTP validation failed. Login aborted." << std::endl;
                            break;
                        }

                        std::cout << "Login successful!" << std::endl;
                        loggedIn = true;

                        // Record login time in LoginHistory table
                        {   
                            // Record login time
                            long long loginTime = std::chrono::duration_cast<std::chrono::seconds>(
                                std::chrono::system_clock::now().time_since_epoch()).count();

                            Aws::DynamoDB::Model::PutItemRequest putRequest;
                            putRequest.SetTableName("LoginHistory");

                            Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue> item;
                            item.emplace("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));
                            item.emplace("login_time", Aws::DynamoDB::Model::AttributeValue().SetN(std::to_string(loginTime)));

                            putRequest.SetItem(std::move(item));

                            auto putOutcome = dynamoClient.PutItem(putRequest);
                            if (!putOutcome.IsSuccess()) {
                                std::cerr << "Failed to record login time: " << putOutcome.GetError().GetMessage() << std::endl;
                            }
                        }
                        // End of login time recording

                        if (loggedIn) {
                            PasswordManager pm(user_id);
                            pm.displayMenu();
                        }

                        break;
                    }
                } else {
                    std::cerr << "Failed to get user data: " << outcome.GetError().GetMessage() << std::endl;
                    attempts++;
                    if (!handleLoginFailureOptions(exitProgram)) {
                        break;
                    }
                    continue;
                }
            } catch (const std::runtime_error& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
                attempts++;
                if (!handleLoginFailureOptions(exitProgram)) {
                    break;
                }
                continue;
            }
        }

        if (!loggedIn && attempts >= max_attempts && !exitProgram) {
            std::cout << "\nMaximum login attempts exceeded." << std::endl;
            if (!handleLoginFailureOptions(exitProgram)) {
                // Do nothing
            }
        }
    }
    // Shutdown AWS SDK
    Aws::ShutdownAPI(options);
}

void sendAccountLockEmail(const std::string& user_email, int lockout_duration_minutes) {
    Aws::Client::ClientConfiguration clientConfig;
    Aws::SES::SESClient sesClient(clientConfig);

    Aws::SES::Model::SendEmailRequest sendEmailRequest;
    Aws::SES::Model::Destination destination;
    destination.AddToAddresses(user_email.c_str());

    sendEmailRequest.SetDestination(destination);

    Aws::SES::Model::Message message;
    Aws::SES::Model::Content subject;
    subject.SetData("Account Locked Notification");
    message.SetSubject(subject);

    Aws::SES::Model::Body body;
    Aws::SES::Model::Content textContent;
    std::string bodyText = "Dear User,\n\nYour account has been locked due to multiple failed login attempts. "
                           "Please try again after " + std::to_string(lockout_duration_minutes) + " minutes.\n\n"
                           "If this wasn't you, please reset your password.\n\nBest regards,\nSecure Login System Team";
    textContent.SetData(bodyText);
    body.SetText(textContent);
    message.SetBody(body);

    sendEmailRequest.SetMessage(message);

    sendEmailRequest.SetSource("harsh.maheshwary@flame.edu.in");

    auto sendEmailOutcome = sesClient.SendEmail(sendEmailRequest);

    if (!sendEmailOutcome.IsSuccess()) {
        std::cerr << "Failed to send account lock email: "
                  << sendEmailOutcome.GetError().GetMessage() << std::endl;
    } else {
        std::cout << "An email notification has been sent to your registered email address." << std::endl;
    }
}

bool handleLoginFailureOptions(bool& exitProgram) {
    std::string choice;
    bool validChoice = false;

    while (!validChoice) {
        std::cout << "Would you like to:" << std::endl;
        std::cout << "1) Forgot Password" << std::endl;
        std::cout << "2) Go back to the main menu (Existing/New User)" << std::endl;
        std::cout << "3) Quit" << std::endl;
        std::cout << "Enter 1, 2, or 3: ";
        std::getline(std::cin, choice);

        if (choice == "1") {
            validChoice = true;
            forgotPassword();
            return true;
        } else if (choice == "2") {
            validChoice = true;
            exitProgram = false;
            return false;
        } else if (choice == "3") {
            validChoice = true;
            std::cout << "Exiting the program. Goodbye!" << std::endl;
            exitProgram = true;
            return false;
        } else {
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
    return false;
}



