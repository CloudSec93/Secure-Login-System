
// // main.cpp
// #include <iostream>
// #include <string>

// #include "new_user.h"      
// #include "existing_user.h" 

// int main() {
//     std::string choice;
//     std::cout << "Are you an existing user or a new user? (existing/new): ";
//     std::getline(std::cin, choice);

//     if (choice == "new") {
//         registerNewUser(); 
//     } else if (choice == "existing") {
//         loginExistingUser(); 
//     } else {
//         std::cout << "Invalid choice." << std::endl;
//     }

//     return 0;
// }





// main.cpp
#include <iostream>
#include <string>

#include "new_user.h"
#include "existing_user.h"

int main() {
    bool exitProgram = false;
    while (!exitProgram) {
        std::string choice;
        std::cout << "Are you an existing user or a new user? (existing/new) or type 'quit' to exit: ";
        std::getline(std::cin, choice);

        if (choice == "new") {
            registerNewUser();
        } else if (choice == "existing") {
            loginExistingUser();
        } else if (choice == "quit") {
            std::cout << "Exiting the program. Goodbye!" << std::endl;
            exitProgram = true;
        } else {
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }

    return 0;
}

