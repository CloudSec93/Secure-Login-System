#include "Encryption.h"
#include <stdexcept>
#include <cctype>

constexpr int ASCII_START = 32;  
constexpr int ASCII_END = 126;   
constexpr int ASCII_RANGE = ASCII_END - ASCII_START + 1;

std::string CaesarCipher::encrypt(const std::string& text, const std::string& key) {
    int shift = std::stoi(key) % ASCII_RANGE;
    std::string result;
    for (char ch : text) {
        if (ch >= ASCII_START && ch <= ASCII_END) {
            char encrypted_char = ASCII_START + (ch - ASCII_START + shift) % ASCII_RANGE;
            result += encrypted_char;
        } else {
            result += ch;
        }
    }
    return result;
}

std::string CaesarCipher::decrypt(const std::string& text, const std::string& key) {
    int shift = std::stoi(key) % ASCII_RANGE;
    std::string result;
    for (char ch : text) {
        if (ch >= ASCII_START && ch <= ASCII_END) {
            char decrypted_char = ASCII_START + (ch - ASCII_START - shift + ASCII_RANGE) % ASCII_RANGE;
            result += decrypted_char;
        } else {
            result += ch;
        }
    }
    return result;
}

std::string VigenereCipher::encrypt(const std::string& text, const std::string& key) {
    std::string result;
    size_t key_length = key.length();
    if (key_length == 0) throw std::invalid_argument("Key cannot be empty");

    for (size_t i = 0; i < text.length(); ++i) {
        char ch = text[i];
        char key_char = key[i % key_length];
        if ((ch >= ASCII_START && ch <= ASCII_END) && (key_char >= ASCII_START && key_char <= ASCII_END)) {
            int shift = key_char - ASCII_START;
            char encrypted_char = ASCII_START + (ch - ASCII_START + shift) % ASCII_RANGE;
            result += encrypted_char;
        } else {
            result += ch;
        }
    }
    return result;
}

std::string VigenereCipher::decrypt(const std::string& text, const std::string& key) {
    std::string result;
    size_t key_length = key.length();
    if (key_length == 0) throw std::invalid_argument("Key cannot be empty");

    for (size_t i = 0; i < text.length(); ++i) {
        char ch = text[i];
        char key_char = key[i % key_length];
        if ((ch >= ASCII_START && ch <= ASCII_END) && (key_char >= ASCII_START && key_char <= ASCII_END)) {
            int shift = key_char - ASCII_START;
            char decrypted_char = ASCII_START + (ch - ASCII_START - shift + ASCII_RANGE) % ASCII_RANGE;
            result += decrypted_char;
        } else {
            result += ch;
        }
    }
    return result;
}
