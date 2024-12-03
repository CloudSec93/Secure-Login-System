
// PasswordManager.h
#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include "Encryption.h"

struct PasswordEntry {
    std::string user_id;
    std::string site_name_account;
    std::string encryption_mechanism;
    std::string key;
    std::string password;
    std::string url;
    std::string user_email;
    std::string user_name;
};

class PasswordManager {
public:
    PasswordManager(const std::string& user_id);
    
    void displayMenu();
    void viewPasswords();
    void addPassword();
    void updatePassword();
    void deletePassword();
    
private:
    std::string user_id_;
    std::string filename_;
    std::vector<PasswordEntry> entries_;
    
    void loadPasswords();
    void savePasswords();
    std::unique_ptr<Encryption> getEncryptionHandler(const std::string& mechanism);
    bool promptForEntry(PasswordEntry& entry);
    void printEntry(const PasswordEntry& entry);
};

#endif // PASSWORD_MANAGER_H
