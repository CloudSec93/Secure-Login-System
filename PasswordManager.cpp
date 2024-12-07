// PasswordManager.cpp
#include "PasswordManager.h"
#include "Encryption.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <string>
#include <cstdlib>

#include <aws/core/Aws.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/QueryRequest.h>
#include <aws/dynamodb/model/AttributeValue.h>

static inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

PasswordManager::PasswordManager(const std::string& user_id)
    : user_id_(user_id), filename_("passwords_" + user_id + ".txt") {
    loadPasswords();
}

void PasswordManager::displayMenu() {
    while (true) {
        std::cout << "\nPassword Manager Menu:\n";
        std::cout << "1) View Passwords\n";
        std::cout << "2) Add Password\n";
        std::cout << "3) Update Password\n";
        std::cout << "4) Delete Password\n";
        std::cout << "5) View Login History\n"; 
        std::cout << "6) Exit Password Manager\n";
        std::cout << "Enter your choice (1-6): ";

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            viewPasswords();
        } else if (choice == "2") {
            addPassword();
        } else if (choice == "3") {
            updatePassword();
        } else if (choice == "4") {
            deletePassword();
        } else if (choice == "5") {
            viewLoginHistory(); 
        } else if (choice == "6") {
            std::cout << "Exiting Password Manager.\n";
            break;
        } else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

void PasswordManager::loadPasswords() {
    entries_.clear();
    std::ifstream infile(filename_);
    if (!infile.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        PasswordEntry entry;
        std::getline(ss, entry.user_id, '|');
        std::getline(ss, entry.site_name_account, '|');
        std::getline(ss, entry.encryption_mechanism, '|');
        std::getline(ss, entry.key, '|');
        std::getline(ss, entry.password, '|');
        std::getline(ss, entry.url, '|');
        std::getline(ss, entry.user_email, '|');
        std::getline(ss, entry.user_name, '|');

        auto encryption = getEncryptionHandler(entry.encryption_mechanism);
        if (encryption) {
            try {
                entry.password = encryption->decrypt(entry.password, entry.key);
            } catch (const std::exception& e) {
                std::cerr << "Error decrypting password for " << entry.site_name_account << ": " << e.what() << "\n";
                continue;
            }
        } else {
            std::cerr << "Unsupported encryption mechanism for entry: " << entry.site_name_account << "\n";
            continue;
        }

        entries_.push_back(entry);
    }
    infile.close();
}

void PasswordManager::savePasswords() {
    std::ofstream outfile(filename_, std::ofstream::trunc);
    if (!outfile.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename_ << "\n";
        return;
    }

    for (const auto& entry : entries_) {
        PasswordEntry encryptedEntry = entry;
        auto encryption = getEncryptionHandler(encryptedEntry.encryption_mechanism);
        if (encryption) {
            try {
                encryptedEntry.password = encryption->encrypt(encryptedEntry.password, encryptedEntry.key);
            } catch (const std::exception& e) {
                std::cerr << "Error encrypting password for " << encryptedEntry.site_name_account << ": " << e.what() << "\n";
                continue;
            }
        } else {
            std::cerr << "Unsupported encryption mechanism for entry: " << encryptedEntry.site_name_account << "\n";
            continue;
        }

        outfile << encryptedEntry.user_id << "|"
                << encryptedEntry.site_name_account << "|"
                << encryptedEntry.encryption_mechanism << "|"
                << encryptedEntry.key << "|"
                << encryptedEntry.password << "|"
                << encryptedEntry.url << "|"
                << encryptedEntry.user_email << "|"
                << encryptedEntry.user_name << "\n";
    }
    outfile.close();
}

std::unique_ptr<Encryption> PasswordManager::getEncryptionHandler(const std::string& mechanism) {
    if (mechanism == "caesar") {
        return std::unique_ptr<Encryption>(new CaesarCipher());
    } else if (mechanism == "vigenere") {
        return std::unique_ptr<Encryption>(new VigenereCipher());
    } else {
        return nullptr;
    }
}

// Strong Password Generator Function
std::string PasswordManager::generateStrongPassword(int length) {
    const std::string uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string lowercase = "abcdefghijklmnopqrstuvwxyz";
    const std::string digits = "0123456789";
    const std::string special = "!@#$%^&*()-_=+[]{}|;:'\",.<>?/`~";

    const std::string all_characters = uppercase + lowercase + digits + special;

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, all_characters.size() - 1);

    std::string password;

    std::uniform_int_distribution<> dist_upper(0, uppercase.size() - 1);
    std::uniform_int_distribution<> dist_lower(0, lowercase.size() - 1);
    std::uniform_int_distribution<> dist_digit(0, digits.size() - 1);
    std::uniform_int_distribution<> dist_special(0, special.size() - 1);

    password += uppercase[dist_upper(generator)];
    password += lowercase[dist_lower(generator)];
    password += digits[dist_digit(generator)];
    password += special[dist_special(generator)];

    for (int i = 4; i < length; ++i) {
        password += all_characters[dist(generator)];
    }

    std::shuffle(password.begin(), password.end(), generator);

    return password;
}

// the following clipboard function will only work on linux.
void PasswordManager::copyToClipboard(const std::string& text) {
    auto isCommandInstalled = [](const std::string& cmd) -> bool {
        std::string checkCmd = "which " + cmd + " > /dev/null 2>&1";
        return system(checkCmd.c_str()) == 0;
    };

    std::string command;
    if (isCommandInstalled("xclip")) {
        command = "echo \"" + text + "\" | xclip -selection clipboard";
    } else if (isCommandInstalled("xsel")) {
        command = "echo \"" + text + "\" | xsel --clipboard --input";
    } else if (isCommandInstalled("wl-copy")) { 
        command = "echo \"" + text + "\" | wl-copy";
    } else {
        std::cerr << "Error: No clipboard utility found (xclip, xsel, wl-copy). Please install one to enable clipboard functionality.\n";
        return;
    }

    if (system(command.c_str()) != 0) {
        std::cerr << "Error: Failed to copy to clipboard using the available clipboard utility.\n";
    } else {
        std::cout << "Password has been copied to the clipboard.\n";
    }
}

bool PasswordManager::promptForEntry(PasswordEntry& entry) {
    std::cout << "Enter site name/account: ";
    std::getline(std::cin, entry.site_name_account);
    if (entry.site_name_account.empty()) {
        std::cout << "Site name/account cannot be empty.\n";
        return false;
    }

    std::cout << "Choose encryption mechanism (caesar/vigenere): ";
    std::getline(std::cin, entry.encryption_mechanism);
    std::transform(entry.encryption_mechanism.begin(), entry.encryption_mechanism.end(), entry.encryption_mechanism.begin(), ::tolower);
    if (entry.encryption_mechanism != "caesar" && entry.encryption_mechanism != "vigenere") {
        std::cout << "Unsupported encryption mechanism.\n";
        return false;
    }

    std::cout << "Enter key for encryption: ";
    std::getline(std::cin, entry.key);
    if (entry.key.empty()) {
        std::cout << "Key cannot be empty.\n";
        return false;
    }

    std::cout << "Do you want to generate a strong password? (yes/no): ";
    std::string generate_choice;
    std::getline(std::cin, generate_choice);
    std::transform(generate_choice.begin(), generate_choice.end(), generate_choice.begin(), ::tolower);

    if (generate_choice == "yes" || generate_choice == "y") {
        int length = 12; 
        std::cout << "Enter desired password length (minimum 8): ";
        std::string length_str;
        std::getline(std::cin, length_str);

        try {
            int input_length = std::stoi(length_str);
            if (input_length >= 8) {
                length = input_length;
            } else {
                std::cout << "Using default length of 12.\n";
            }
        } catch (...) {
            std::cout << "Invalid input. Using default length of 12.\n";
        }

        entry.password = generateStrongPassword(length);
        std::cout << "Generated Password: " << entry.password << "\n";

        copyToClipboard(entry.password);
    } else {
        std::cout << "Enter password: ";
        std::getline(std::cin, entry.password);
        if (entry.password.empty()) {
            std::cout << "Password cannot be empty.\n";
            return false;
        }
    }

    std::cout << "Enter URL: ";
    std::getline(std::cin, entry.url);
    if (entry.url.empty()) {
        std::cout << "URL cannot be empty.\n";
        return false;
    }

    std::cout << "Enter user email: ";
    std::getline(std::cin, entry.user_email);
    if (entry.user_email.empty()) {
        std::cout << "User email cannot be empty.\n";
        return false;
    }

    std::cout << "Enter user name: ";
    std::getline(std::cin, entry.user_name);
    if (entry.user_name.empty()) {
        std::cout << "User name cannot be empty.\n";
        return false;
    }

    entry.user_id = user_id_;
    return true;
}

void PasswordManager::addPassword() {
    PasswordEntry newEntry;
    if (!promptForEntry(newEntry)) {
        std::cout << "Failed to add password due to incomplete information.\n";
        return;
    }

    auto it = std::find_if(entries_.begin(), entries_.end(),
                           [&](const PasswordEntry& e) { return e.site_name_account == newEntry.site_name_account; });
    if (it != entries_.end()) {
        std::cout << "An entry for this site/account already exists.\n";
        return;
    }

    entries_.push_back(newEntry);
    savePasswords();
    std::cout << "Password added successfully.\n";
}

void PasswordManager::viewPasswords() {
    if (entries_.empty()) {
        std::cout << "No passwords stored.\n";
        return;
    }

    std::cout << "\nStored Passwords:\n";
    for (const auto& entry : entries_) {
        printEntry(entry);
    }
}

void PasswordManager::printEntry(const PasswordEntry& entry) {
    std::cout << "----------------------------------------\n";
    std::cout << "Site/Account: " << entry.site_name_account << "\n";
    std::cout << "URL: " << entry.url << "\n";
    std::cout << "User Email: " << entry.user_email << "\n";
    std::cout << "User Name: " << entry.user_name << "\n";
    std::cout << "Password: " << entry.password << "\n";
    std::cout << "Encryption Mechanism: " << entry.encryption_mechanism << "\n";
    std::cout << "----------------------------------------\n";
}

void PasswordManager::updatePassword() {
    std::cout << "Enter the site name/account of the password to update: ";
    std::string site;
    std::getline(std::cin, site);

    auto it = std::find_if(entries_.begin(), entries_.end(),
                           [&](const PasswordEntry& e) { return e.site_name_account == site; });
    if (it == entries_.end()) {
        std::cout << "No entry found for the specified site/account.\n";
        return;
    }

    std::cout << "Do you want to update the encryption mechanism and key? (yes/no): ";
    std::string update_encryption_choice;
    std::getline(std::cin, update_encryption_choice);
    std::transform(update_encryption_choice.begin(), update_encryption_choice.end(), update_encryption_choice.begin(), ::tolower);

    if (update_encryption_choice == "yes" || update_encryption_choice == "y") {
        std::cout << "Choose new encryption mechanism (caesar/vigenere): ";
        std::getline(std::cin, it->encryption_mechanism);
        std::transform(it->encryption_mechanism.begin(), it->encryption_mechanism.end(), it->encryption_mechanism.begin(), ::tolower);
        if (it->encryption_mechanism != "caesar" && it->encryption_mechanism != "vigenere") {
            std::cout << "Unsupported encryption mechanism.\n";
            return;
        }

        std::cout << "Enter new key for encryption: ";
        std::getline(std::cin, it->key);
        if (it->key.empty()) {
            std::cout << "Key cannot be empty.\n";
            return;
        }
    }

    std::cout << "Do you want to generate a strong password? (yes/no): ";
    std::string generate_choice;
    std::getline(std::cin, generate_choice);
    std::transform(generate_choice.begin(), generate_choice.end(), generate_choice.begin(), ::tolower);

    if (generate_choice == "yes" || generate_choice == "y") {
        int length = 12; // Default password length
        std::cout << "Enter desired password length (minimum 8): ";
        std::string length_str;
        std::getline(std::cin, length_str);

        try {
            int input_length = std::stoi(length_str);
            if (input_length >= 8) {
                length = input_length;
            } else {
                std::cout << "Using default length of 12.\n";
            }
        } catch (...) {
            std::cout << "Invalid input. Using default length of 12.\n";
        }

        it->password = generateStrongPassword(length);
        std::cout << "Generated Password: " << it->password << "\n";

        // Copy the password to the clipboard
        copyToClipboard(it->password);
    } else {
        std::cout << "Enter new password: ";
        std::string new_password;
        std::getline(std::cin, new_password);
        if (new_password.empty()) {
            std::cout << "Password cannot be empty.\n";
            return;
        }
        it->password = new_password;
    }

    savePasswords();
    std::cout << "Password updated successfully.\n";
}

void PasswordManager::deletePassword() {
    std::cout << "Enter the site name/account of the password to delete: ";
    std::string site;
    std::getline(std::cin, site);

    auto it = std::find_if(entries_.begin(), entries_.end(),
                           [&](const PasswordEntry& e) { return e.site_name_account == site; });
    if (it == entries_.end()) {
        std::cout << "No entry found for the specified site/account.\n";
        return;
    }

    entries_.erase(it);
    savePasswords();
    std::cout << "Password deleted successfully.\n";
}

void PasswordManager::viewLoginHistory() {
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        Aws::Client::ClientConfiguration clientConfig;

        Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);

        const int pageSize = 5; 
        int totalDisplayed = 0;
        const int maxEntries = 20;
        bool moreEntries = true;
        Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue> lastEvaluatedKey;

        while (moreEntries && totalDisplayed < maxEntries) {
            Aws::DynamoDB::Model::QueryRequest queryRequest;
            queryRequest.SetTableName("LoginHistory");
            queryRequest.SetKeyConditionExpression("user_id = :u");
            queryRequest.AddExpressionAttributeValues(":u", Aws::DynamoDB::Model::AttributeValue(user_id_));

            queryRequest.SetScanIndexForward(false);
            queryRequest.SetLimit(pageSize);

            if (!lastEvaluatedKey.empty()) {
                queryRequest.SetExclusiveStartKey(lastEvaluatedKey);
            }

            auto queryOutcome = dynamoClient.Query(queryRequest);
            if (!queryOutcome.IsSuccess()) {
                std::cerr << "Failed to retrieve login history: " << queryOutcome.GetError().GetMessage() << std::endl;
                break;
            }

            const auto& items = queryOutcome.GetResult().GetItems();
            if (items.empty()) {
                if (totalDisplayed == 0) {
                    std::cout << "No login history available.\n";
                } else {
                    std::cout << "No more login history entries.\n";
                }
                break;
            }

            std::cout << "\nLogin History (Most Recent First):\n";
            for (const auto& item : items) {
                if (item.find("login_time") != item.end()) {
                    long long loginTime = std::stoll(item.at("login_time").GetN());
                    time_t loginTime_t = static_cast<time_t>(loginTime);
                    std::cout << "- " << std::ctime(&loginTime_t);
                    totalDisplayed++;
                    if (totalDisplayed >= maxEntries) {
                        moreEntries = false;
                        break;
                    }
                }
            }

            if (queryOutcome.GetResult().GetLastEvaluatedKey().empty() || totalDisplayed >= maxEntries) {
                moreEntries = false;
            } else {
                std::string choice;
                std::cout << "\nView next " << pageSize << " entries? (yes/no): ";
                std::getline(std::cin, choice);
                std::transform(choice.begin(), choice.end(), choice.begin(), ::tolower); 

                if (choice == "yes" || choice == "y") {
                    lastEvaluatedKey = queryOutcome.GetResult().GetLastEvaluatedKey();
                } else {
                    moreEntries = false;
                }
            }
        }
    }
    Aws::ShutdownAPI(options);
}




