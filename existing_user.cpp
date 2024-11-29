// existing_user.cpp
#include "existing_user.h"
#include "new_user.h"          
#include "forgot_password.h" 
#include "CustomHash.h"
#include "otp.h"

#include <aws/core/Aws.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <iostream>
#include <string>

bool handleLoginFailureOptions(bool& exitProgram);

void loginExistingUser() {
    const int max_attempts = 3;
    int attempts = 0;
    bool loggedIn = false;
    bool exitProgram = false;

    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        Aws::Client::ClientConfiguration clientConfig;
        Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);

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

                    std::string salt = item.at("salt").GetS();
                    std::string storedHashedPassword = item.at("hashed_password").GetS();
                    std::string user_email = item.at("user_email").GetS();

                    std::string saltedPassword = salt + password;

                    CustomHash hasher;
                    unsigned long long hashedPasswordValue = hasher.computeHash(saltedPassword);
                    std::string hashedPassword = hasher.hashToHex(hashedPasswordValue);

                    if (hashedPassword != storedHashedPassword) {
                        std::cout << "Incorrect password." << std::endl;
                        attempts++;
                        if (!handleLoginFailureOptions(exitProgram)) {
                            break; 
                        }
                        continue; 
                    }

                    std::cout << "Password verified. An OTP has been sent to your registered email." << std::endl;

                    if (!validateOTP(user_email)) {
                        std::cout << "OTP validation failed. Login aborted." << std::endl;
                        break; 
                    }

                    std::cout << "Login successful!" << std::endl;
                    loggedIn = true;
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
 
            }
        }
    }
    Aws::ShutdownAPI(options);

}

// Function definition
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
