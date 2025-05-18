// Argon2Hash.h
#ifndef ARGON2HASH_H
#define ARGON2HASH_H

#include <cstdint>   
#include <string>
#include <vector>

class Argon2Hash {
public:
    static std::vector<uint8_t> generateSalt(size_t length);
    static std::string toHex(const std::vector<uint8_t>& data);
    static std::vector<uint8_t> fromHex(const std::string& hex);
    static std::string computeHash(const std::string& password, const std::vector<uint8_t>& salt);
};

#endif // ARGON2HASH_H
