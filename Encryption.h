// Encryption.h
#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>

class Encryption {
public:
    virtual ~Encryption() {}
    virtual std::string encrypt(const std::string& plaintext, const std::string& key) = 0;
    virtual std::string decrypt(const std::string& ciphertext, const std::string& key) = 0;
};

class CaesarCipher : public Encryption {
public:
    std::string encrypt(const std::string& plaintext, const std::string& key) override;
    std::string decrypt(const std::string& ciphertext, const std::string& key) override;
};

class VigenereCipher : public Encryption {
public:
    std::string encrypt(const std::string& plaintext, const std::string& key) override;
    std::string decrypt(const std::string& ciphertext, const std::string& key) override;
};

#endif // ENCRYPTION_H
