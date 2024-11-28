// // #include <aws/core/Aws.h>
// // #include <aws/dynamodb/DynamoDBClient.h>
// // #include <aws/dynamodb/model/ListTablesRequest.h>
// // #include <iostream>

// // int main() {
// //     Aws::SDKOptions options;
// //     Aws::InitAPI(options);
// //     {
// //         Aws::DynamoDB::DynamoDBClient dynamoClient;
// //         Aws::DynamoDB::Model::ListTablesRequest request;

// //         auto outcome = dynamoClient.ListTables(request);

// //         if (outcome.IsSuccess()) {
// //             std::cout << "Your DynamoDB Tables areeeeeee:" << std::endl;
// //             for (const auto& tableName : outcome.GetResult().GetTableNames()) {
// //                 std::cout << tableName << std::endl;
// //             }
// //         } else {
// //             std::cerr << "Failed to list tables: " 
// //                       << outcome.GetError().GetMessage() << std::endl;
// //         }
// //     }
// //     Aws::ShutdownAPI(options);
// //     return 0;
// // }






// // main.cpp
// #include <iostream>
// #include <string>

// #include "new_user.h" // Include the header for new user registration

// int main() {
//     std::string choice;
//     std::cout << "Are you an existing user or a new user? (existing/new): ";
//     std::getline(std::cin, choice);

//     if (choice == "new") {
//         registerNewUser(); // Call the function from new_user.cpp
//     } else if (choice == "existing") {
//         std::cout << "Coming soon" << std::endl;
//     } else {
//         std::cout << "Invalid choice." << std::endl;
//     }

//     return 0;
// }



// main.cpp
#include <iostream>
#include <string>

#include "new_user.h"      // Include the header for new user registration
#include "existing_user.h" // Include the header for existing user login

int main() {
    std::string choice;
    std::cout << "Are you an existing user or a new user? (existing/new): ";
    std::getline(std::cin, choice);

    if (choice == "new") {
        registerNewUser(); // Call the function from new_user.cpp
    } else if (choice == "existing") {
        loginExistingUser(); // Call the function from existing_user.cpp
    } else {
        std::cout << "Invalid choice." << std::endl;
    }

    return 0;
}



