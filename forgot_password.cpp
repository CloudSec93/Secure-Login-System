// // // forgot_password.cpp
// // #include "forgot_password.h"
// // #include "CustomHash.h"
// // #include "otp.h"

// // #include <aws/core/Aws.h>
// // #include <aws/dynamodb/DynamoDBClient.h>
// // #include <aws/dynamodb/model/UpdateItemRequest.h>
// // #include <aws/dynamodb/model/GetItemRequest.h>
// // #include <iostream>
// // #include <string>

// // void forgotPassword() {
// //     Aws::SDKOptions options;
// //     Aws::InitAPI(options);
// //     {
// //         Aws::Client::ClientConfiguration clientConfig;
// //         Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);

// //         std::string user_id;

// //         std::cout << "Enter your user_id: ";
// //         std::getline(std::cin, user_id);

// //         // Retrieve user data
// //         try {
// //             Aws::DynamoDB::Model::GetItemRequest getRequest;
// //             getRequest.SetTableName("SLS_Table");
// //             getRequest.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

// //             auto getOutcome = dynamoClient.GetItem(getRequest);

// //             if (getOutcome.IsSuccess()) {
// //                 const auto& item = getOutcome.GetResult().GetItem();
// //                 if (item.empty()) {
// //                     std::cerr << "Error: user_id does not exist." << std::endl;
// //                     return;
// //                 }

// //                 if (item.find("user_email") == item.end()) {
// //                     std::cerr << "Error: Email not found for this user." << std::endl;
// //                     return;
// //                 }

// //                 std::string user_email = item.at("user_email").GetS();

// //                 // Send OTP to user's email
// //                 std::cout << "An OTP has been sent to your registered email." << std::endl;

// //                 if (!validateOTP(user_email)) {
// //                     std::cout << "OTP validation failed. Cannot reset password." << std::endl;
// //                     return;
// //                 }

// //                 // OTP validated, proceed to reset password
// //                 std::string new_password;
// //                 std::string confirm_password;

// //                 std::cout << "Enter your new password: ";
// //                 std::getline(std::cin, new_password);

// //                 std::cout << "Re-enter your new password: ";
// //                 std::getline(std::cin, confirm_password);

// //                 if (new_password != confirm_password) {
// //                     std::cerr << "Error: Passwords do not match. Password reset aborted." << std::endl;
// //                     return;
// //                 }

// //                 // Generate new salt and hash the new password
// //                 std::string new_salt = generateSalt(16); // 16-character salt
// //                 std::string saltedPassword = new_salt + new_password;

// //                 CustomHash hasher;
// //                 unsigned long long hashedPasswordValue = hasher.computeHash(saltedPassword);
// //                 std::string new_hashedPassword = hasher.hashToHex(hashedPasswordValue);

// //                 // Update the password and salt in the database
// //                 Aws::DynamoDB::Model::UpdateItemRequest updateRequest;
// //                 updateRequest.SetTableName("SLS_Table");
// //                 updateRequest.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

// //                 // Prepare the attribute updates
// //                 Aws::DynamoDB::Model::AttributeValueUpdate passwordUpdate;
// //                 passwordUpdate.SetValue(Aws::DynamoDB::Model::AttributeValue(new_hashedPassword));
// //                 passwordUpdate.SetAction(Aws::DynamoDB::Model::AttributeAction::PUT);

// //                 Aws::DynamoDB::Model::AttributeValueUpdate saltUpdate;
// //                 saltUpdate.SetValue(Aws::DynamoDB::Model::AttributeValue(new_salt));
// //                 saltUpdate.SetAction(Aws::DynamoDB::Model::AttributeAction::PUT);

// //                 updateRequest.AddAttributeUpdates("hashed_password", passwordUpdate);
// //                 updateRequest.AddAttributeUpdates("salt", saltUpdate);

// //                 auto updateOutcome = dynamoClient.UpdateItem(updateRequest);

// //                 if (updateOutcome.IsSuccess()) {
// //                     std::cout << "Your password has been successfully reset." << std::endl;
// //                 } else {
// //                     std::cerr << "Failed to update password: " << updateOutcome.GetError().GetMessage() << std::endl;
// //                 }

// //             } else {
// //                 std::cerr << "Failed to get user data: " << getOutcome.GetError().GetMessage() << std::endl;
// //                 return;
// //             }
// //         } catch (const std::runtime_error& e) {
// //             std::cerr << "Exception: " << e.what() << std::endl;
// //             return;
// //         }
// //     }
// //     Aws::ShutdownAPI(options);
// // }



// // forgot_password.cpp
// #include "forgot_password.h"
// #include "CustomHash.h"
// #include "otp.h"

// #include <aws/core/Aws.h>
// #include <aws/dynamodb/DynamoDBClient.h>
// #include <aws/dynamodb/model/UpdateItemRequest.h>
// #include <aws/dynamodb/model/GetItemRequest.h>
// #include <iostream>
// #include <string>
// #include <limits>

// void forgotPassword() {
//     Aws::SDKOptions options;
//     Aws::InitAPI(options);
//     {
//         Aws::Client::ClientConfiguration clientConfig;
//         Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);

//         std::string user_id;

//         std::cout << "Enter your user_id: ";
//         std::getline(std::cin, user_id);

//         // Retrieve user data
//         try {
//             Aws::DynamoDB::Model::GetItemRequest getRequest;
//             getRequest.SetTableName("SLS_Table");
//             getRequest.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

//             auto getOutcome = dynamoClient.GetItem(getRequest);

//             if (getOutcome.IsSuccess()) {
//                 const auto& item = getOutcome.GetResult().GetItem();
//                 if (item.empty()) {
//                     std::cerr << "Error: user_id does not exist." << std::endl;
//                     return;
//                 }

//                 if (item.find("user_email") == item.end()) {
//                     std::cerr << "Error: Email not found for this user." << std::endl;
//                     return;
//                 }

//                 std::string user_email = item.at("user_email").GetS();

//                 // Send OTP to user's email
//                 std::cout << "An OTP has been sent to your registered email." << std::endl;

//                 if (!validateOTP(user_email)) {
//                     std::cout << "OTP validation failed. Cannot reset password." << std::endl;
//                     return;
//                 }

//                 // Clear the input buffer before reading passwords
//                 std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

//                 // OTP validated, proceed to reset password
//                 std::string new_password;
//                 std::string confirm_password;

//                 std::cout << "Enter your new password: ";
//                 std::getline(std::cin, new_password);

//                 std::cout << "Re-enter your new password: ";
//                 std::getline(std::cin, confirm_password);

//                 if (new_password != confirm_password) {
//                     std::cerr << "Error: Passwords do not match. Password reset aborted." << std::endl;
//                     return;
//                 }

//                 // Generate new salt and hash the new password
//                 std::string new_salt = generateSalt(16); // Implement generateSalt function
//                 std::string saltedPassword = new_salt + new_password;

//                 CustomHash hasher;
//                 unsigned long long hashedPasswordValue = hasher.computeHash(saltedPassword);
//                 std::string new_hashedPassword = hasher.hashToHex(hashedPasswordValue);

//                 // Update the password and salt in the database
//                 Aws::DynamoDB::Model::UpdateItemRequest updateRequest;
//                 updateRequest.SetTableName("SLS_Table");
//                 updateRequest.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

//                 // Prepare the attribute updates
//                 Aws::DynamoDB::Model::AttributeValueUpdate passwordUpdate;
//                 passwordUpdate.SetValue(Aws::DynamoDB::Model::AttributeValue(new_hashedPassword));
//                 passwordUpdate.SetAction(Aws::DynamoDB::Model::AttributeAction::PUT);

//                 Aws::DynamoDB::Model::AttributeValueUpdate saltUpdate;
//                 saltUpdate.SetValue(Aws::DynamoDB::Model::AttributeValue(new_salt));
//                 saltUpdate.SetAction(Aws::DynamoDB::Model::AttributeAction::PUT);

//                 updateRequest.AddAttributeUpdates("hashed_password", passwordUpdate);
//                 updateRequest.AddAttributeUpdates("salt", saltUpdate);

//                 auto updateOutcome = dynamoClient.UpdateItem(updateRequest);

//                 if (updateOutcome.IsSuccess()) {
//                     std::cout << "Your password has been successfully reset." << std::endl;
//                 } else {
//                     std::cerr << "Failed to update password: " << updateOutcome.GetError().GetMessage() << std::endl;
//                 }

//             } else {
//                 std::cerr << "Failed to get user data: " << getOutcome.GetError().GetMessage() << std::endl;
//                 return;
//             }
//         } catch (const std::runtime_error& e) {
//             std::cerr << "Exception: " << e.what() << std::endl;
//             return;
//         }
//     }
//     Aws::ShutdownAPI(options);
// }






// forgot_password.cpp
#include "forgot_password.h"
#include "CustomHash.h"
#include "Argon2Hash.h"
#include "otp.h"

#include <aws/core/Aws.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/UpdateItemRequest.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <iostream>
#include <string>
#include <limits>

void forgotPassword() {
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        Aws::Client::ClientConfiguration clientConfig;
        Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);

        std::string user_id;

        std::cout << "Enter your user_id: ";
        std::getline(std::cin, user_id);

        try {
            Aws::DynamoDB::Model::GetItemRequest getRequest;
            getRequest.SetTableName("SLS_Table");
            getRequest.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

            auto getOutcome = dynamoClient.GetItem(getRequest);

            if (getOutcome.IsSuccess()) {
                const auto& item = getOutcome.GetResult().GetItem();
                if (item.empty()) {
                    std::cerr << "Error: user_id does not exist." << std::endl;
                    return;
                }

                if (item.find("user_email") == item.end()) {
                    std::cerr << "Error: Email not found for this user." << std::endl;
                    return;
                }

                if (item.find("hashing_algo") == item.end()) {
                    std::cerr << "Error: Hashing algorithm not specified for this user." << std::endl;
                    return;
                }

                std::string user_email = item.at("user_email").GetS();
                std::string hashing_algo = item.at("hashing_algo").GetS();

                std::cout << "An OTP has been sent to your registered email." << std::endl;

                if (!validateOTP(user_email)) {
                    std::cout << "OTP validation failed. Cannot reset password." << std::endl;
                    return;
                }

 
                std::string new_password;
                std::string confirm_password;

                std::cout << "Enter your new password: ";
                std::getline(std::cin, new_password);

                std::cout << "Re-enter your new password: ";
                std::getline(std::cin, confirm_password);

                if (new_password != confirm_password) {
                    std::cerr << "Error: Passwords do not match. Password reset aborted." << std::endl;
                    return;
                }

                std::string new_hashedPassword;
                std::string new_salt;

                if (hashing_algo == "scratch") {
                    new_salt = generateSalt(16); 
                    std::string saltedPassword = new_salt + new_password;

                    CustomHash hasher;
                    unsigned long long hashedPasswordValue = hasher.computeHash(saltedPassword);
                    new_hashedPassword = hasher.hashToHex(hashedPasswordValue);
                } else if (hashing_algo == "argon2") {
                    size_t salt_length = 16; 
                    std::vector<uint8_t> salt_vec = Argon2Hash::generateSalt(salt_length);

                    new_hashedPassword = Argon2Hash::computeHash(new_password, salt_vec);

                    new_salt = Argon2Hash::toHex(salt_vec);
                } else {
                    std::cerr << "Error: Unsupported hashing algorithm." << std::endl;
                    return;
                }

                Aws::DynamoDB::Model::UpdateItemRequest updateRequest;
                updateRequest.SetTableName("SLS_Table");
                updateRequest.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

                Aws::DynamoDB::Model::AttributeValueUpdate passwordUpdate;
                passwordUpdate.SetValue(Aws::DynamoDB::Model::AttributeValue(new_hashedPassword));
                passwordUpdate.SetAction(Aws::DynamoDB::Model::AttributeAction::PUT);

                Aws::DynamoDB::Model::AttributeValueUpdate saltUpdate;
                saltUpdate.SetValue(Aws::DynamoDB::Model::AttributeValue(new_salt));
                saltUpdate.SetAction(Aws::DynamoDB::Model::AttributeAction::PUT);


                updateRequest.AddAttributeUpdates("hashed_password", passwordUpdate);
                updateRequest.AddAttributeUpdates("salt", saltUpdate);

                auto updateOutcome = dynamoClient.UpdateItem(updateRequest);

                if (updateOutcome.IsSuccess()) {
                    std::cout << "Your password has been successfully reset." << std::endl;
                } else {
                    std::cerr << "Failed to update password: " << updateOutcome.GetError().GetMessage() << std::endl;
                }

            } else {
                std::cerr << "Failed to get user data: " << getOutcome.GetError().GetMessage() << std::endl;
                return;
            }
        } catch (const std::runtime_error& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            return;
        }
    }
    Aws::ShutdownAPI(options);
}
