// // new_user.cpp
// #include <aws/core/Aws.h>
// #include <aws/dynamodb/DynamoDBClient.h>
// #include <aws/dynamodb/model/GetItemRequest.h>
// #include <aws/dynamodb/model/QueryRequest.h>
// #include <aws/dynamodb/model/PutItemRequest.h>
// #include <aws/dynamodb/model/AttributeValue.h>
// #include <iostream>
// #include <regex>
// #include <stdexcept>
// #include <string>

// #include "CustomHash.h"  // Include the header for the hashing module

// // Function to validate email format using regex
// bool isValidEmail(const std::string& email) {
//     const std::regex emailRegex(R"((\w+)(\.{0,1})(\w*)@(\w+)(\.(\w+))+)"); // Simple regex for email validation
//     return std::regex_match(email, emailRegex);
// }

// // Function to check if the user_id is unique in DynamoDB
// bool checkUniqueUserId(Aws::DynamoDB::DynamoDBClient& dynamoClient, const std::string& user_id) {
//     Aws::DynamoDB::Model::GetItemRequest request;
//     request.SetTableName("SLS_Table");
//     request.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

//     auto outcome = dynamoClient.GetItem(request);
//     if (outcome.IsSuccess()) {
//         return outcome.GetResult().GetItem().empty(); // True if no item exists with this user_id
//     } else {
//         throw std::runtime_error("Failed to query user_id: " + outcome.GetError().GetMessage());
//     }
// }

// // Function to check if the user_email is unique in DynamoDB using a Global Secondary Index (GSI)
// bool checkUniqueEmail(Aws::DynamoDB::DynamoDBClient& dynamoClient, const std::string& user_email) {
//     Aws::DynamoDB::Model::QueryRequest queryRequest;
//     queryRequest.SetTableName("SLS_Table");
//     queryRequest.SetIndexName("user_email-index"); // Ensure this matches your GSI name
//     queryRequest.SetKeyConditionExpression("user_email = :email");
//     queryRequest.AddExpressionAttributeValues(":email", Aws::DynamoDB::Model::AttributeValue(user_email));

//     auto outcome = dynamoClient.Query(queryRequest);
//     if (outcome.IsSuccess()) {
//         return outcome.GetResult().GetItems().empty(); // True if no item exists with this email
//     } else {
//         throw std::runtime_error("Failed to query user_email: " + outcome.GetError().GetMessage());
//     }
// }

// int main() {
//     Aws::SDKOptions options;
//     Aws::InitAPI(options);
//     {
//         Aws::Client::ClientConfiguration clientConfig;
//         Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);

//         std::string user_id, user_name, user_email, password;

//         // Prompt for user input
//         std::cout << "Enter user_id: ";
//         std::getline(std::cin, user_id);

//         std::cout << "Enter user_name: ";
//         std::getline(std::cin, user_name);

//         std::cout << "Enter user_email: ";
//         std::getline(std::cin, user_email);

//         std::cout << "Enter password: ";
//         std::getline(std::cin, password);

//         // Validate email format
//         if (!isValidEmail(user_email)) {
//             std::cerr << "Invalid email format. Please try again." << std::endl;
//             Aws::ShutdownAPI(options);
//             return 1;
//         }

//         try {
//             // Check uniqueness of user_id
//             if (!checkUniqueUserId(dynamoClient, user_id)) {
//                 std::cout << "Error: user_id already exists. Please choose a unique user_id." << std::endl;
//                 Aws::ShutdownAPI(options);
//                 return 1;
//             }

//             // Check uniqueness of user_email
//             if (!checkUniqueEmail(dynamoClient, user_email)) {
//                 std::cout << "Error: user_email already exists. Please choose a unique email." << std::endl;
//                 Aws::ShutdownAPI(options);
//                 return 1;
//             }

//             // All validations passed; proceed to generate salt and hash password
//             std::string salt = generateSalt(16); // 16-character salt
//             std::string saltedPassword = salt + password;

//             CustomHash hasher;
//             unsigned long long hashedPasswordValue = hasher.computeHash(saltedPassword);
//             std::string hashedPassword = hasher.hashToHex(hashedPasswordValue);

//             // Prepare to insert into DynamoDB
//             Aws::DynamoDB::Model::PutItemRequest putRequest;
//             putRequest.SetTableName("SLS_Table");

//             // Set user_id attribute
//             putRequest.AddItem("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

//             // Set user_name attribute
//             putRequest.AddItem("user_name", Aws::DynamoDB::Model::AttributeValue(user_name));

//             // Set user_email attribute
//             putRequest.AddItem("user_email", Aws::DynamoDB::Model::AttributeValue(user_email));

//             // Set hashed_password attribute
//             putRequest.AddItem("hashed_password", Aws::DynamoDB::Model::AttributeValue(hashedPassword));

//             // Set salt attribute
//             putRequest.AddItem("salt", Aws::DynamoDB::Model::AttributeValue(salt));

//             // Set hashing_algo attribute
//             putRequest.AddItem("hashing_algo", Aws::DynamoDB::Model::AttributeValue("scratch"));

//             // Execute the PutItem request
//             auto putOutcome = dynamoClient.PutItem(putRequest);
//             if (putOutcome.IsSuccess()) {
//                 std::cout << "Success: User has been added to the database!" << std::endl;
//             } else {
//                 std::cerr << "Failed to add user to the database: "
//                           << putOutcome.GetError().GetMessage() << std::endl;
//                 Aws::ShutdownAPI(options);
//                 return 1;
//             }

//         } catch (const std::runtime_error& e) {
//             std::cerr << "Exception: " << e.what() << std::endl;
//             Aws::ShutdownAPI(options);
//             return 1;
//         }
//     }
//     Aws::ShutdownAPI(options);
//     return 0;
// }




// // new_user.cpp
// #include "new_user.h"
// #include "CustomHash.h"

// #include <aws/core/Aws.h>
// #include <aws/dynamodb/DynamoDBClient.h>
// #include <aws/dynamodb/model/GetItemRequest.h>
// #include <aws/dynamodb/model/QueryRequest.h>
// #include <aws/dynamodb/model/PutItemRequest.h>
// #include <aws/dynamodb/model/AttributeValue.h>
// #include <iostream>
// #include <regex>
// #include <stdexcept>
// #include <string>

// // Function to validate email format using regex
// bool isValidEmail(const std::string& email) {
//     const std::regex emailRegex(R"((\w+)(\.{0,1})(\w*)@(\w+)(\.(\w+))+)"); // Simple regex for email validation
//     return std::regex_match(email, emailRegex);
// }

// // Function to check if the user_id is unique in DynamoDB
// bool checkUniqueUserId(Aws::DynamoDB::DynamoDBClient& dynamoClient, const std::string& user_id) {
//     Aws::DynamoDB::Model::GetItemRequest request;
//     request.SetTableName("SLS_Table");
//     request.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

//     auto outcome = dynamoClient.GetItem(request);
//     if (outcome.IsSuccess()) {
//         return outcome.GetResult().GetItem().empty(); // True if no item exists with this user_id
//     } else {
//         throw std::runtime_error("Failed to query user_id: " + outcome.GetError().GetMessage());
//     }
// }

// // Function to check if the user_email is unique in DynamoDB using a Global Secondary Index (GSI)
// bool checkUniqueEmail(Aws::DynamoDB::DynamoDBClient& dynamoClient, const std::string& user_email) {
//     Aws::DynamoDB::Model::QueryRequest queryRequest;
//     queryRequest.SetTableName("SLS_Table");
//     queryRequest.SetIndexName("user_email-index"); // Ensure this matches your GSI name
//     queryRequest.SetKeyConditionExpression("user_email = :email");
//     queryRequest.AddExpressionAttributeValues(":email", Aws::DynamoDB::Model::AttributeValue(user_email));

//     auto outcome = dynamoClient.Query(queryRequest);
//     if (outcome.IsSuccess()) {
//         return outcome.GetResult().GetItems().empty(); // True if no item exists with this email
//     } else {
//         throw std::runtime_error("Failed to query user_email: " + outcome.GetError().GetMessage());
//     }
// }

// void registerNewUser() {
//     Aws::SDKOptions options;
//     Aws::InitAPI(options);
//     {
//         Aws::Client::ClientConfiguration clientConfig;
//         Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);

//         std::string user_id, user_name, user_email, password;

//         // Prompt for user input
//         std::cout << "Enter user_id: ";
//         std::getline(std::cin, user_id);

//         std::cout << "Enter user_name: ";
//         std::getline(std::cin, user_name);

//         std::cout << "Enter user_email: ";
//         std::getline(std::cin, user_email);

//         std::cout << "Enter password: ";
//         std::getline(std::cin, password);

//         // Validate email format
//         if (!isValidEmail(user_email)) {
//             std::cerr << "Invalid email format. Please try again." << std::endl;
//             Aws::ShutdownAPI(options);
//             return;
//         }

//         try {
//             // Check uniqueness of user_id
//             if (!checkUniqueUserId(dynamoClient, user_id)) {
//                 std::cout << "Error: user_id already exists. Please choose a unique user_id." << std::endl;
//                 Aws::ShutdownAPI(options);
//                 return;
//             }

//             // Check uniqueness of user_email
//             if (!checkUniqueEmail(dynamoClient, user_email)) {
//                 std::cout << "Error: user_email already exists. Please choose a unique email." << std::endl;
//                 Aws::ShutdownAPI(options);
//                 return;
//             }

//             // All validations passed; proceed to generate salt and hash password
//             std::string salt = generateSalt(16); // 16-character salt
//             std::string saltedPassword = salt + password;

//             CustomHash hasher;
//             unsigned long long hashedPasswordValue = hasher.computeHash(saltedPassword);
//             std::string hashedPassword = hasher.hashToHex(hashedPasswordValue);

//             // Prepare to insert into DynamoDB
//             Aws::DynamoDB::Model::PutItemRequest putRequest;
//             putRequest.SetTableName("SLS_Table");

//             // Set attributes
//             putRequest.AddItem("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));
//             putRequest.AddItem("user_name", Aws::DynamoDB::Model::AttributeValue(user_name));
//             putRequest.AddItem("user_email", Aws::DynamoDB::Model::AttributeValue(user_email));
//             putRequest.AddItem("hashed_password", Aws::DynamoDB::Model::AttributeValue(hashedPassword));
//             putRequest.AddItem("salt", Aws::DynamoDB::Model::AttributeValue(salt));
//             putRequest.AddItem("hashing_algo", Aws::DynamoDB::Model::AttributeValue("scratch"));

//             // Execute the PutItem request
//             auto putOutcome = dynamoClient.PutItem(putRequest);
//             if (putOutcome.IsSuccess()) {
//                 std::cout << "Success: User has been added to the database!" << std::endl;
//             } else {
//                 std::cerr << "Failed to add user to the database: "
//                           << putOutcome.GetError().GetMessage() << std::endl;
//                 Aws::ShutdownAPI(options);
//                 return;
//             }

//         } catch (const std::runtime_error& e) {
//             std::cerr << "Exception: " << e.what() << std::endl;
//             Aws::ShutdownAPI(options);
//             return;
//         }
//     }
//     Aws::ShutdownAPI(options);
// }







// new_user.cpp
#include "new_user.h"
#include "CustomHash.h"
#include "otp.h"

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

// Function to validate email format using regex
bool isValidEmail(const std::string& email) {
    const std::regex emailRegex(R"((\w+)(\.{0,1})(\w*)@(\w+)(\.(\w+))+)"); // Simple regex for email validation
    return std::regex_match(email, emailRegex);
}

// Function to check if the user_id is unique in DynamoDB
bool checkUniqueUserId(Aws::DynamoDB::DynamoDBClient& dynamoClient, const std::string& user_id) {
    Aws::DynamoDB::Model::GetItemRequest request;
    request.SetTableName("SLS_Table");
    request.AddKey("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));

    auto outcome = dynamoClient.GetItem(request);
    if (outcome.IsSuccess()) {
        return outcome.GetResult().GetItem().empty(); // True if no item exists with this user_id
    } else {
        throw std::runtime_error("Failed to query user_id: " + outcome.GetError().GetMessage());
    }
}

// Function to check if the user_email is unique in DynamoDB using a Global Secondary Index (GSI)
bool checkUniqueEmail(Aws::DynamoDB::DynamoDBClient& dynamoClient, const std::string& user_email) {
    Aws::DynamoDB::Model::QueryRequest queryRequest;
    queryRequest.SetTableName("SLS_Table");
    queryRequest.SetIndexName("user_email-index"); // Ensure this matches your GSI name
    queryRequest.SetKeyConditionExpression("user_email = :email");
    queryRequest.AddExpressionAttributeValues(":email", Aws::DynamoDB::Model::AttributeValue(user_email));

    auto outcome = dynamoClient.Query(queryRequest);
    if (outcome.IsSuccess()) {
        return outcome.GetResult().GetItems().empty(); // True if no item exists with this email
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

        // Prompt for user input
        std::cout << "Enter user_id: ";
        std::getline(std::cin, user_id);

        std::cout << "Enter user_name: ";
        std::getline(std::cin, user_name);

        std::cout << "Enter user_email: ";
        std::getline(std::cin, user_email);

        std::cout << "Enter password: ";
        std::getline(std::cin, password);

        // Validate email format
        if (!isValidEmail(user_email)) {
            std::cerr << "Invalid email format. Please try again." << std::endl;
            Aws::ShutdownAPI(options);
            return;
        }

        try {
            // Check uniqueness of user_id
            if (!checkUniqueUserId(dynamoClient, user_id)) {
                std::cout << "Error: user_id already exists. Please choose a unique user_id." << std::endl;
                Aws::ShutdownAPI(options);
                return;
            }

            // Check uniqueness of user_email
            if (!checkUniqueEmail(dynamoClient, user_email)) {
                std::cout << "Error: user_email already exists. Please choose a unique email." << std::endl;
                Aws::ShutdownAPI(options);
                return;
            }

            // Send OTP and validate
            std::cout << "An OTP has been sent to your email. Please enter it to proceed." << std::endl;
            if (!validateOTP(user_email)) {
                std::cout << "OTP validation failed. Registration aborted." << std::endl;
                Aws::ShutdownAPI(options);
                return;
            }

            // All validations passed; proceed to generate salt and hash password
            std::string salt = generateSalt(16); // 16-character salt
            std::string saltedPassword = salt + password;

            CustomHash hasher;
            unsigned long long hashedPasswordValue = hasher.computeHash(saltedPassword);
            std::string hashedPassword = hasher.hashToHex(hashedPasswordValue);

            // Prepare to insert into DynamoDB
            Aws::DynamoDB::Model::PutItemRequest putRequest;
            putRequest.SetTableName("SLS_Table");

            // Set attributes
            putRequest.AddItem("user_id", Aws::DynamoDB::Model::AttributeValue(user_id));
            putRequest.AddItem("user_name", Aws::DynamoDB::Model::AttributeValue(user_name));
            putRequest.AddItem("user_email", Aws::DynamoDB::Model::AttributeValue(user_email));
            putRequest.AddItem("hashed_password", Aws::DynamoDB::Model::AttributeValue(hashedPassword));
            putRequest.AddItem("salt", Aws::DynamoDB::Model::AttributeValue(salt));
            putRequest.AddItem("hashing_algo", Aws::DynamoDB::Model::AttributeValue("scratch"));

            // Execute the PutItem request
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
