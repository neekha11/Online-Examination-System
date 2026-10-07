#include "../../include/utils/Validator.h"

bool Validator::isValidUsername(string username) {
    return !username.empty();
}

bool Validator::isValidPassword(string password) {
    return password.length() >= 4;
}