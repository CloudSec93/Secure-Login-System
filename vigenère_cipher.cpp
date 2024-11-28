// #include <iostream>
// #include <string>

// using namespace std;

// string generateKey(string str, string key) {
//     int x = str.size();

//     for (int i = 0; ; i++) {
//         if (x == i){
//             i = 0;}
//         if (key.size() == str.size())
//             break;
//         key.push_back(key[i]);
//     }
//     return key;
// }

// string encrypt(string str, string key) {
//     string cipher_text;

//     for (int i = 0; i < str.size(); i++) {
//         char c = str[i];
//         if (isalpha(c)) {
//             char base = isupper(c) ? 'A' : 'a';
//             char k = isupper(key[i]) ? key[i] - 'A' : key[i] - 'a';
//             c = (c - base + k) % 26 + base;
//         }
//         cipher_text.push_back(c);
//     }
//     return cipher_text;
// }

// string decrypt(string cipher_text, string key) {
//     string orig_text;

//     for (int i = 0; i < cipher_text.size(); i++) {
//         char c = cipher_text[i];
//         if (isalpha(c)) {
//             char base = isupper(c) ? 'A' : 'a';
//             char k = isupper(key[i]) ? key[i] - 'A' : key[i] - 'a';
//             c = (c - base - k + 26) % 26 + base;
//         }
//         orig_text.push_back(c);
//     }
//     return orig_text;
// }

// int main() {
//     string str;
//     cout << "Enter the password (plaintext): ";
//     getline(cin, str);

//     string keyword = "CSIT";

//     string key = generateKey(str, keyword);

//     string cipher_text = encrypt(str, key);

//     string decrypted_text = decrypt(cipher_text, key);

//     cout << "\nKey: " << keyword << endl;
//     cout << "Generated repeating key: " << key << endl;
//     cout << "Encrypted text: " << cipher_text << endl;
//     cout << "Decrypted text: " << decrypted_text << endl;

//     return 0;
// }
