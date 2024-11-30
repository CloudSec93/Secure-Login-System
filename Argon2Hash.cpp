// Argon2Hash.cpp
#include "Argon2Hash.h"
extern "C" {
    #include <argon2.h>
}
#include <iostream>
#include <vector>
#include <random>
#include <cstring>
#include <iomanip>
#include <sstream>

std::vector<uint8_t> Argon2Hash::generateSalt(size_t length) {
    std::vector<uint8_t> salt(length);
    std::random_device rd;
    std::uniform_int_distribution<uint8_t> dist(0, 255);
    for (size_t i = 0; i < length; ++i) {
        salt[i] = dist(rd);
    }
    return salt;
}

std::string Argon2Hash::toHex(const std::vector<uint8_t>& data) {
    std::ostringstream oss;
    for (auto byte : data) {
        oss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(byte);
    }
    return oss.str();
}

std::vector<uint8_t> Argon2Hash::fromHex(const std::string& hex) {
    std::vector<uint8_t> data;
    data.reserve(hex.size() / 2);
    for (size_t i = 0; i < hex.size(); i += 2) {
        uint8_t byte = static_cast<uint8_t>(std::stoi(hex.substr(i, 2), nullptr, 16));
        data.push_back(byte);
    }
    return data;
}

std::string Argon2Hash::computeHash(const std::string& password, const std::vector<uint8_t>& salt) {
    // Argon2 parameters
    uint32_t t_cost = 3;        // Number of iterations
    uint32_t m_cost = 1 << 16;  // Memory usage in kibibytes (64 MiB)
    uint32_t parallelism = 1;   // Number of threads and compute lanes
    uint32_t hash_length = 32;  // Length of the hash in bytes (256 bits)

    std::vector<uint8_t> hash(hash_length);

    int result = argon2id_hash_raw(
        t_cost,
        m_cost,
        parallelism,
        password.data(),
        password.size(),
        salt.data(),
        salt.size(),
        hash.data(),
        hash.size()
    );

    if (result != ARGON2_OK) {
        throw std::runtime_error("Error hashing password: " + std::string(argon2_error_message(result)));
    }

    // Convert hash to hexadecimal string
    return toHex(hash);
}
