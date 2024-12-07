// PasswordManager.h
#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include "Encryption.h"

// AWS SDK Headers for DynamoDB
#include <aws/core/Aws.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/QueryRequest.h>
#include <aws/dynamodb/model/AttributeValue.h>
#include <chrono>

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
    void viewLoginHistory(); 
    
private:
    std::string user_id_;
    std::string filename_;
    std::vector<PasswordEntry> entries_;
    
    void loadPasswords();
    void savePasswords();
    std::unique_ptr<Encryption> getEncryptionHandler(const std::string& mechanism);
    bool promptForEntry(PasswordEntry& entry);
    void printEntry(const PasswordEntry& entry);
    
    // Helper methods
    std::string generateStrongPassword(int length);
    void copyToClipboard(const std::string& text);
};

#endif // PASSWORD_MANAGER_H