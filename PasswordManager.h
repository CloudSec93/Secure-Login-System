#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <string>

class PasswordManager {
private:
    std::string userId;

    // Private helper functions for encryption
    std::string caesarEncrypt(const std::string& plaintext, int key);
    std::string caesarDecrypt(const std::string& ciphertext, int key);
    std::string generateVigenereKey(const std::string& str, const std::string& key);
    std::string vigenereEncrypt(const std::string& str, const std::string& key);
    std::string vigenereDecrypt(const std::string& cipher_text, const std::string& key);

public:
    // Constructor
    explicit PasswordManager(const std::string& userId);

    // Public member functions
    void addPassword();
    void viewPasswords();
};

#endif // PASSWORDMANAGER_H
