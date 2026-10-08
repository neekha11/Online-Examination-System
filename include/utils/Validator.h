#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>
using namespace std;

class Validator {
public:
    static bool isValidUsername(string username);
    static bool isValidPassword(string password);
};

#endif