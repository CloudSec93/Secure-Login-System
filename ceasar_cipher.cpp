#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

string encrypt(string plaintext, int key) {
    string ciphertext = "";
    for (char c : plaintext) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            c = (c - base + key) % 26 + base;
        }
        ciphertext += c;
    }
    return ciphertext;
}

string decrypt(string ciphertext, int key) {
    string plaintext = "";
    for (char c : ciphertext) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            c = (c - base - key + 26) % 26 + base;
        }
        plaintext += c;
    }
    return plaintext;
}

int main() {
    string plaintext;
    cout << "Enter the password (plaintext): ";
    getline(cin, plaintext);

    srand(time(0));

    int key = rand() % 25 + 1;

    string ciphertext = encrypt(plaintext, key);
    string decryptedtext = decrypt(ciphertext, key);

    cout << "\nKey: " << key << endl;
    cout << "Encrypted text: " << ciphertext << endl;
    cout << "Decrypted text: " << decryptedtext << endl;

    return 0;
}
