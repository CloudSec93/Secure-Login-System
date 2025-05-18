// CustomHash.cpp
#include "CustomHash.h"
#include <iomanip>
#include <sstream>
#include <random>
#include "utilities.h"

unsigned long long CustomHash::computeHash(const std::string &input) {
    unsigned long long hash = 0xcbf29ce484222325ULL; //hexadecimal constant  // ULL means treat this value as an unsigned long long

    for (size_t i = 0; i < input.size(); ++i) {
        hash ^= static_cast<unsigned char>(input[i]); //converts the character to an unsigned char
        hash *= 0x100000001b3ULL; // prime number in a hex value
        hash ^= (hash << 13);
        hash ^= (hash >> 7);
        hash ^= (hash << 17);
    }

    hash ^= (hash >> 33);
    hash *= 0xff51afd7ed558ccdULL; // another large prime number in hex value
    hash ^= (hash >> 33);
    hash *= 0xc4ceb9fe1a85ec53ULL; // another large prime number in hex value
    hash ^= (hash >> 33);

    return hash;
}

std::string CustomHash::hashToHex(unsigned long long hashValue) const {
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hashValue;   //setw(16) only affects the min width not max. so if its more than 16 characters then it will store all of them. it wont truncate them.
    return ss.str(); // return this hex value as a string
}
