// // // Encryption.cpp
// // #include "Encryption.h"
// // #include <cctype>

// // // Caesar Cipher Implementation
// // std::string CaesarCipher::encrypt(const std::string& plaintext, const std::string& key) {
// //     if (key.empty()) return plaintext;
// //     int shift = std::stoi(key) % 26;
// //     std::string ciphertext = plaintext;
// //     for (char& c : ciphertext) {
// //         if (std::isalpha(c)) {
// //             char base = std::islower(c) ? 'a' : 'A';
// //             c = static_cast<char>(base + (c - base + shift) % 26);
// //         }
// //     }
// //     return ciphertext;
// // }

// // std::string CaesarCipher::decrypt(const std::string& ciphertext, const std::string& key) {
// //     if (key.empty()) return ciphertext;
// //     int shift = std::stoi(key) % 26;
// //     std::string plaintext = ciphertext;
// //     for (char& c : plaintext) {
// //         if (std::isalpha(c)) {
// //             char base = std::islower(c) ? 'a' : 'A';
// //             c = static_cast<char>(base + (c - base - shift + 26) % 26);
// //         }
// //     }
// //     return plaintext;
// // }




#include "Encryption.h"
#include <cctype>

// Caesar Cipher Implementation
std::string CaesarCipher::encrypt(const std::string& plaintext, const std::string& key) {
    if (key.empty()) return plaintext;
    int shift = std::stoi(key) % 95; // ASCII printable characters range (32-126)
    std::string ciphertext = plaintext;
    for (char& c : ciphertext) {
        if (std::isprint(c)) { // Encrypt all printable ASCII characters
            c = static_cast<char>(32 + (c - 32 + shift) % 95); // Wrap around printable range
        }
    }
    return ciphertext;
}

std::string CaesarCipher::decrypt(const std::string& ciphertext, const std::string& key) {
    if (key.empty()) return ciphertext;
    int shift = std::stoi(key) % 95; // ASCII printable characters range (32-126)
    std::string plaintext = ciphertext;
    for (char& c : plaintext) {
        if (std::isprint(c)) { // Decrypt all printable ASCII characters
            c = static_cast<char>(32 + (c - 32 - shift + 95) % 95); // Wrap around printable range
        }
    }
    return plaintext;
}


// Vigenère Cipher Implementation
std::string VigenereCipher::encrypt(const std::string& plaintext, const std::string& key) {
    if (key.empty()) return plaintext;
    std::string ciphertext = plaintext;
    int key_length = key.length();
    for (size_t i = 0; i < ciphertext.length(); ++i) {
        if (std::isalpha(ciphertext[i])) {
            char base = std::islower(ciphertext[i]) ? 'a' : 'A';
            char key_char = key[i % key_length];
            if (!std::isalpha(key_char)) key_char = 'A'; // Default to 'A' if key character is not alphabetic
            int key_shift = std::tolower(key_char) - 'a';
            ciphertext[i] = static_cast<char>(base + (ciphertext[i] - base + key_shift) % 26);
        }
    }
    return ciphertext;
}

std::string VigenereCipher::decrypt(const std::string& ciphertext, const std::string& key) {
    if (key.empty()) return ciphertext;
    std::string plaintext = ciphertext;
    int key_length = key.length();
    for (size_t i = 0; i < plaintext.length(); ++i) {
        if (std::isalpha(plaintext[i])) {
            char base = std::islower(plaintext[i]) ? 'a' : 'A';
            char key_char = key[i % key_length];
            if (!std::isalpha(key_char)) key_char = 'A'; // Default to 'A' if key character is not alphabetic
            int key_shift = std::tolower(key_char) - 'a';
            plaintext[i] = static_cast<char>(base + (plaintext[i] - base - key_shift + 26) % 26);
        }
    }
    return plaintext;
}




