// #ifndef EXISTING_USER_H
// #define EXISTING_USER_H

// void loginExistingUser();

// #endif 



// // existing_user.h
// #ifndef EXISTING_USER_H
// #define EXISTING_USER_H

// void loginExistingUser();

// #endif // EXISTING_USER_H





// existing_user.h
#ifndef EXISTING_USER_H
#define EXISTING_USER_H
#include <string>
void loginExistingUser();
bool handleLoginFailureOptions(bool& exitProgram);
void sendAccountLockEmail(const std::string& user_email, int lockout_duration_minutes);

#endif // EXISTING_USER_H
