//Updated Caesar cipher to handle numbers and special characters. 
//i changed the code to include all ascii codes (32 to 126) and not just alphabets.
//for eg, if the input is "HarshM2005@", every character will be encrypted incl 2005 and @.  
//based on initial stress test, it looks good, however i have seen if the key is 32 and the password is in all caps then basically the encrypted password becomes the same password just in lower caps prob because of ascii table



#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

string encrypt(string plaintext, int key) {
    string ciphertext = "";
    for (char c : plaintext) {
        if (c >= 32 && c <= 126) { 
            c = (c - 32 + key) % 95 + 32;
        }
        ciphertext += c;
    }
    return ciphertext;
}

string decrypt(string ciphertext, int key) {
    string plaintext = "";
    for (char c : ciphertext) {
        if (c >= 32 && c <= 126) { 
            c = (c - 32 - key + 95) % 95 + 32;
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

    int key = rand() % 94 + 1;

    string ciphertext = encrypt(plaintext, key);
    string decryptedtext = decrypt(ciphertext, key);

    cout << "\nKey: " << key << endl;
    cout << "Encrypted text: " << ciphertext << endl;
    cout << "Decrypted text: " << decryptedtext << endl;

    return 0;
}


