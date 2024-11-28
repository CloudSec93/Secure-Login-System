// existing_user.cpp
#include "existing_user.h"
#include "CustomHash.h"
#include "otp.h"

#include <aws/core/Aws.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <iostream>
#include <string>

void loginExistingUser() {
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        Aws::Client::ClientConfiguration clientConfig;
        Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);

        std::string user_id, password;

        // Prompt for user input
        std::cout << "Enter user_id: ";
        std::getline(std::cin, user_id);

        std::cout << "Enter password: ";
        std::getline(std::cin, password);

        try {
            // Create GetItem request to retrieve the user data
            Aws::DynamoDB::Model::GetItemRequest request;
            request.SetTableName("SLS_Table");
            request.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

            auto outcome = dynamoClient.GetItem(request);

            if (outcome.IsSuccess()) {
                const auto& item = outcome.GetResult().GetItem();
                if (item.empty()) {
                    std::cerr << "Error: user_id does not exist." << std::endl;
                    Aws::ShutdownAPI(options);
                    return;
                }

                // Retrieve salt, hashed_password, and user_email
                if (item.find("salt") == item.end() ||
                    item.find("hashed_password") == item.end() ||
                    item.find("user_email") == item.end()) {
                    std::cerr << "Error: Incomplete user data in database." << std::endl;
                    Aws::ShutdownAPI(options);
                    return;
                }

                std::string salt = item.at("salt").GetS();
                std::string storedHashedPassword = item.at("hashed_password").GetS();
                std::string user_email = item.at("user_email").GetS();

                // Combine the retrieved salt with the entered password
                std::string saltedPassword = salt + password;

                // Hash the combination using the same algorithm
                CustomHash hasher;
                unsigned long long hashedPasswordValue = hasher.computeHash(saltedPassword);
                std::string hashedPassword = hasher.hashToHex(hashedPasswordValue);

                // Compare the hashes
                if (hashedPassword != storedHashedPassword) {
                    std::cout << "Incorrect password." << std::endl;
                    Aws::ShutdownAPI(options);
                    return;
                }

                // Password matches, proceed to send OTP
                std::cout << "Password verified. An OTP has been sent to your registered email." << std::endl;

                // Validate OTP
                if (!validateOTP(user_email)) {
                    std::cout << "OTP validation failed. Login aborted." << std::endl;
                    Aws::ShutdownAPI(options);
                    return;
                }

                // OTP validation successful
                std::cout << "Login successful!" << std::endl;

            } else {
                std::cerr << "Failed to get user data: " << outcome.GetError().GetMessage() << std::endl;
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
