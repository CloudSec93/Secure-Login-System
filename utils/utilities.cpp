// utilities.cpp
#include "utilities.h"
#include <chrono>
#include <ctime>
#include <random>

long long getCurrentUnixTimestamp() {
    return std::chrono::seconds(std::time(nullptr)).count();
}

std::string generateSalt(size_t length) {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string salt;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    for (size_t i = 0; i < length; ++i) {
        salt += charset[dist(rng)];
    }
    return salt;
}