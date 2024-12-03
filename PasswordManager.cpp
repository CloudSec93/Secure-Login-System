// PasswordManager.cpp
#include "PasswordManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
// #include "MakeUnique.h"



// Helper function to trim whitespace
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
        std::cout << "5) Exit Password Manager\n";
        std::cout << "Enter your choice (1-5): ";
        
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
            entry.password = encryption->decrypt(entry.password, entry.key);
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
            encryptedEntry.password = encryption->encrypt(encryptedEntry.password, encryptedEntry.key);
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

// std::unique_ptr<Encryption> PasswordManager::getEncryptionHandler(const std::string& mechanism) {
//     if (mechanism == "caesar") {
//         return std::make_unique<CaesarCipher>();
//     } else if (mechanism == "vigenere") {
//         return std::make_unique<VigenereCipher>();
//     } else {
//         return nullptr;
//     }
// }


std::unique_ptr<Encryption> PasswordManager::getEncryptionHandler(const std::string& mechanism) {
    if (mechanism == "caesar") {
        return std::unique_ptr<Encryption>(new CaesarCipher());
    } else if (mechanism == "vigenere") {
        return std::unique_ptr<Encryption>(new VigenereCipher());
    } else {
        return nullptr;
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
    
    std::cout << "Enter password: ";
    std::getline(std::cin, entry.password);
    if (entry.password.empty()) {
        std::cout << "Password cannot be empty.\n";
        return false;
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
    
    // Check for duplicate site_name_account
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
    
    std::cout << "Enter new password: ";
    std::string new_password;
    std::getline(std::cin, new_password);
    if (new_password.empty()) {
        std::cout << "Password cannot be empty.\n";
        return;
    }
    
    it->password = new_password;
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










