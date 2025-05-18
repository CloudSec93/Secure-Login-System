// new_user.cpp
#include "auth/new_user.h"
#include "hashing/CustomHash.h"
#include "hashing/Argon2Hash.h"
#include "other_features/otp.h"

#include <aws/core/Aws.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <aws/dynamodb/model/QueryRequest.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/AttributeValue.h>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <chrono>

bool isValidEmail(const std::string& email) {
    const std::regex emailRegex(R"((\w+)(\.{0,1})(\w*)@(\w+)(\.(\w+))+)"); 
    return std::regex_match(email, emailRegex);
}

bool checkUniqueUserId(Aws::DynamoDB::DynamoDBClient& dynamoClient, const std::string& user_id) {
    Aws::DynamoDB::Model::GetItemRequest request;
    request.SetTableName("SLS_Table");
    request.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

    auto outcome = dynamoClient.GetItem(request);
    if (outcome.IsSuccess()) {
        return outcome.GetResult().GetItem().empty(); 
    } else {
        throw std::runtime_error("Failed to query user_id: " + outcome.GetError().GetMessage());
    }
}

bool checkUniqueEmail(Aws::DynamoDB::DynamoDBClient& dynamoClient, const std::string& user_email) {
    Aws::DynamoDB::Model::QueryRequest queryRequest;
    queryRequest.SetTableName("SLS_Table");
    queryRequest.SetIndexName("user_email-index"); 
    queryRequest.SetKeyConditionExpression("user_email = :email");
    queryRequest.AddExpressionAttributeValues(":email", Aws::DynamoDB::Model::AttributeValue(user_email));

    auto outcome = dynamoClient.Query(queryRequest);
    if (outcome.IsSuccess()) {
        return outcome.GetResult().GetItems().empty();  
    } else {
        throw std::runtime_error("Failed to query user_email: " + outcome.GetError().GetMessage());
    }
}

void registerNewUser() {
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        Aws::Client::ClientConfiguration clientConfig;
        Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);

        std::string user_id, user_name, user_email, password;

        std::cout << "Enter user_id: ";
        std::getline(std::cin, user_id);

        std::cout << "Enter user_name: ";
        std::getline(std::cin, user_name);

        std::cout << "Enter user_email: ";
        std::getline(std::cin, user_email);

        std::cout << "Enter password: ";
        std::getline(std::cin, password);

        if (!isValidEmail(user_email)) {
            std::cerr << "Invalid email format. Please try again." << std::endl;
            Aws::ShutdownAPI(options);
            return;
        }

        try {
            if (!checkUniqueUserId(dynamoClient, user_id)) {
                std::cout << "Error: user_id already exists. Please choose a unique user_id." << std::endl;
                Aws::ShutdownAPI(options);
                return;
            }

            if (!checkUniqueEmail(dynamoClient, user_email)) {
                std::cout << "Error: user_email already exists. Please choose a unique email." << std::endl;
                Aws::ShutdownAPI(options);
                return;
            }

            std::cout << "An OTP has been sent to your email. Please enter it to proceed." << std::endl;
            if (!validateOTP(user_email)) {
                std::cout << "OTP validation failed. Registration aborted." << std::endl;
                Aws::ShutdownAPI(options);
                return;
            }

            std::string hashing_algo;
            std::cout << "Choose hashing algorithm ('argon2' or 'scratch'): ";
            std::getline(std::cin, hashing_algo);

            if (hashing_algo != "argon2" && hashing_algo != "scratch") {
                std::cerr << "Invalid hashing algorithm choice. Please try again." << std::endl;
                Aws::ShutdownAPI(options);
                return;
            }

            std::string hashedPassword;
            std::string salt;

            if (hashing_algo == "scratch") {
                salt = generateSalt(16);                                
                std::string saltedPassword = salt + password;

                CustomHash hasher;
                unsigned long long hashedPasswordValue = hasher.computeHash(saltedPassword);
                hashedPassword = hasher.hashToHex(hashedPasswordValue);

            } else if (hashing_algo == "argon2") {
                size_t salt_length = 16; 
                std::vector<uint8_t> salt_vec = Argon2Hash::generateSalt(salt_length);

                hashedPassword = Argon2Hash::computeHash(password, salt_vec);

                salt = Argon2Hash::toHex(salt_vec);
            }

            int failedAttempts = 0;
            long long lastAttemptTime = 0;
            long long lockoutUntil = 0;

            Aws::DynamoDB::Model::PutItemRequest putRequest;
            putRequest.SetTableName("SLS_Table");

            putRequest.AddItem("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));
            putRequest.AddItem("user_name", Aws::DynamoDB::Model::AttributeValue(user_name));
            putRequest.AddItem("user_email", Aws::DynamoDB::Model::AttributeValue(user_email));
            putRequest.AddItem("hashed_password", Aws::DynamoDB::Model::AttributeValue(hashedPassword));
            putRequest.AddItem("salt", Aws::DynamoDB::Model::AttributeValue(salt));
            putRequest.AddItem("hashing_algo", Aws::DynamoDB::Model::AttributeValue(hashing_algo));

            putRequest.AddItem("FailedAttempts", Aws::DynamoDB::Model::AttributeValue().SetN(std::to_string(failedAttempts)));
            putRequest.AddItem("LastAttemptTime", Aws::DynamoDB::Model::AttributeValue().SetN(std::to_string(lastAttemptTime)));
            putRequest.AddItem("LockoutUntil", Aws::DynamoDB::Model::AttributeValue().SetN(std::to_string(lockoutUntil)));

            auto putOutcome = dynamoClient.PutItem(putRequest);
            if (putOutcome.IsSuccess()) {
                std::cout << "Success: User has been added to the database!" << std::endl;
            } else {
                std::cerr << "Failed to add user to the database: "
                          << putOutcome.GetError().GetMessage() << std::endl;
                Aws::ShutdownAPI(options);
                return;
            }

        } catch (const std::runtime_error& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            Aws::ShutdownAPI(options);
            return;
        }
    }
    Aws::ShutdownAPI(options);
}
