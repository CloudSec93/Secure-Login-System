
// CustomHash.h
#ifndef CUSTOM_HASH_H
#define CUSTOM_HASH_H

#include <string>

class CustomHash {
public:
    unsigned long long computeHash(const std::string &input);

    std::string hashToHex(unsigned long long hashValue) const;
};

std::string generateSalt(size_t length);

#endif
