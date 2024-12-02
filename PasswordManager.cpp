#include "PasswordManager.h"
#include <aws/core/Aws.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/QueryRequest.h>
#include <aws/dynamodb/model/AttributeValue.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <map>
#include <vector>
#include <cctype>

PasswordManager::PasswordManager(const std::string& userId) : userId(userId) {
}

std::string PasswordManager::caesarEncrypt(const std::string& plaintext, int key) {
    std::string ciphertext = "";
    for (char c : plaintext) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            char base = std::islower(static_cast<unsigned char>(c)) ? 'a' : 'A';
            c = (c - base + key) % 26 + base;
        }
        ciphertext += c;
    }
    return ciphertext;
}

std::string PasswordManager::caesarDecrypt(const std::string& ciphertext, int key) {
    std::string plaintext = "";
    for (char c : ciphertext) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            char base = std::islower(static_cast<unsigned char>(c)) ? 'a' : 'A';
            c = (c - base - key + 26) % 26 + base;
        }
        plaintext += c;
    }
    return plaintext;
}

std::string PasswordManager::generateVigenereKey(const std::string& str, const std::string& key) {
    std::string newKey = key;
    int x = str.size();

    for (size_t i = 0; ; i++) {
        if (x == newKey.size())
            break;
        newKey.push_back(newKey[i % key.size()]);
    }
    return newKey;
}

std::string PasswordManager::vigenereEncrypt(const std::string& str, const std::string& key) {
    std::string cipher_text;

    for (size_t i = 0; i < str.size(); i++) {
        char c = str[i];
        if (std::isalpha(static_cast<unsigned char>(c))) {
            char base = std::isupper(static_cast<unsigned char>(c)) ? 'A' : 'a';
            char k = std::isupper(static_cast<unsigned char>(key[i])) ? key[i] - 'A' : key[i] - 'a';
            c = (c - base + k) % 26 + base;
        }
        cipher_text.push_back(c);
    }
    return cipher_text;
}

std::string PasswordManager::vigenereDecrypt(const std::string& cipher_text, const std::string& key) {
    std::string orig_text;

    for (size_t i = 0; i < cipher_text.size(); i++) {
        char c = cipher_text[i];
        if (std::isalpha(static_cast<unsigned char>(c))) {
            char base = std::isupper(static_cast<unsigned char>(c)) ? 'A' : 'a';
            char k = std::isupper(static_cast<unsigned char>(key[i])) ? key[i] - 'A' : key[i] - 'a';
            c = (c - base - k + 26) % 26 + base;
        }
        orig_text.push_back(c);
    }
    return orig_text;
}

void PasswordManager::addPassword() {
    std::string site_name_account;
    std::string password;
    std::string user_email;
    std::string user_name;
    std::string url;
    std::string encryption_mechanism;
    std::string key;

    std::srand(std::time(0));

    // Helper lambda to get non-empty input
    auto getNonEmptyInput = [&](const std::string& prompt, std::string& input) {
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, input);
            if (!input.empty()) {
                break;
            }
            std::cout << "Input cannot be empty. Please try again." << std::endl;
        }
    };

    // Get non-empty inputs
    getNonEmptyInput("Enter the site or account name: ", site_name_account);
    getNonEmptyInput("Enter the password: ", password);
    getNonEmptyInput("Enter the email associated with this account: ", user_email);
    getNonEmptyInput("Enter the username for this account: ", user_name);
    getNonEmptyInput("Enter the URL of the site/account: ", url);

    // Get valid encryption mechanism
    while (true) {
        getNonEmptyInput("Select encryption mechanism (caesar_cipher/vigenère_cipher): ", encryption_mechanism);
        if (encryption_mechanism == "caesar_cipher" || encryption_mechanism == "vigenère_cipher") {
            break;
        }
        std::cout << "Invalid encryption mechanism selected. Please enter 'caesar_cipher' or 'vigenère_cipher'." << std::endl;
    }

    // Encrypt the password based on the selected mechanism
    std::string encrypted_password;
    if (encryption_mechanism == "caesar_cipher") {
        int caesar_key = std::rand() % 25 + 1; // Random key between 1 and 25
        key = std::to_string(caesar_key);
        encrypted_password = caesarEncrypt(password, caesar_key);
    } else if (encryption_mechanism == "vigenère_cipher") {
        std::string keyword = "CSIT"; // You can choose a different keyword or make it dynamic
        key = keyword;
        std::string repeating_key = generateVigenereKey(password, keyword);
        encrypted_password = vigenereEncrypt(password, repeating_key);
    }

    Aws::Client::ClientConfiguration clientConfig;
    Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);

    Aws::DynamoDB::Model::PutItemRequest pir;
    pir.SetTableName("PasswordManager_Table");

    pir.AddItem("user_id", Aws::DynamoDB::Model::AttributeValue(this->userId));
    pir.AddItem("site_name_account", Aws::DynamoDB::Model::AttributeValue(site_name_account));
    pir.AddItem("password", Aws::DynamoDB::Model::AttributeValue(encrypted_password));
    pir.AddItem("user_email", Aws::DynamoDB::Model::AttributeValue(user_email));
    pir.AddItem("user_name", Aws::DynamoDB::Model::AttributeValue(user_name));
    pir.AddItem("url", Aws::DynamoDB::Model::AttributeValue(url));
    pir.AddItem("encryption_mechanism", Aws::DynamoDB::Model::AttributeValue(encryption_mechanism));
    pir.AddItem("key", Aws::DynamoDB::Model::AttributeValue(key));

    auto outcome = dynamoClient.PutItem(pir);
    if (!outcome.IsSuccess()) {
        std::cerr << "Failed to add password to PasswordManager_Table: " << outcome.GetError().GetMessage() << std::endl;
    } else {
        std::cout << "Password successfully added to PasswordManager_Table." << std::endl;
    }
}

void PasswordManager::viewPasswords() {
    Aws::Client::ClientConfiguration clientConfig;
    Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);

    Aws::DynamoDB::Model::QueryRequest queryRequest;
    queryRequest.SetTableName("PasswordManager_Table");
    queryRequest.SetKeyConditionExpression("user_id = :uid");
    queryRequest.AddExpressionAttributeValues(":uid", Aws::DynamoDB::Model::AttributeValue(this->userId));

    auto outcome = dynamoClient.Query(queryRequest);
    if (!outcome.IsSuccess()) {
        std::cerr << "Failed to retrieve passwords: " << outcome.GetError().GetMessage() << std::endl;
        return;
    }

    const auto& items = outcome.GetResult().GetItems();
    if (items.empty()) {
        std::cout << "No passwords found for this user." << std::endl;
        return;
    }

    for (const auto& item : items) {
        std::string site_name_account = item.at("site_name_account").GetS();
        std::string encrypted_password = item.at("password").GetS();
        std::string user_email = item.at("user_email").GetS();
        std::string user_name = item.at("user_name").GetS();
        std::string url = item.at("url").GetS();
        std::string encryption_mechanism = item.at("encryption_mechanism").GetS();
        std::string key = item.at("key").GetS();

        std::string decrypted_password;
        if (encryption_mechanism == "caesar_cipher") {
            int caesar_key = std::stoi(key);
            decrypted_password = caesarDecrypt(encrypted_password, caesar_key);
        } else if (encryption_mechanism == "vigenère_cipher") {
            std::string repeating_key = generateVigenereKey(encrypted_password, key);
            decrypted_password = vigenereDecrypt(encrypted_password, repeating_key);
        }

        std::cout << "\nSite/Account Name: " << site_name_account << std::endl;
        std::cout << "Decrypted Password: " << decrypted_password << std::endl;
        std::cout << "User Email: " << user_email << std::endl;
        std::cout << "User Name: " << user_name << std::endl;
        std::cout << "URL: " << url << std::endl;
    }
}







