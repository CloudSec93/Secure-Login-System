// utilities.h
#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>

// Function to get the current UNIX timestamp
long long getCurrentUnixTimestamp();

// Function to generate a salt string of given length
std::string generateSalt(size_t length);

#endif // UTILITIES_H