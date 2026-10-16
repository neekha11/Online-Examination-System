#include "../../include/utils/Validator.h"
#include <cctype>

bool Validator::isValidUsername(const std::string& username) {
    if (username.size() < 3 || username.size() > 30)
        return false;

    for (unsigned char c : username) {
        if (!std::isalnum(c) && c != '_')
            return false;
    }
    return true;
}

bool Validator::isValidPassword(const std::string& password) {
    if (password.size() < 8)
        return false;

    bool hasLetter = false;
    bool hasDigit = false;

    for (unsigned char c : password) {
        if (std::isalpha(c)) hasLetter = true;
        if (std::isdigit(c)) hasDigit = true;
    }
    return hasLetter && hasDigit;
}

bool Validator::isValidText(const std::string& text, std::size_t maxLength) {
    if (text.empty() || text.size() > maxLength)
        return false;

    if (text.find_first_not_of(" \t") == std::string::npos)
        return false;

    return text.find_first_of(",\n\r") == std::string::npos;
}

bool Validator::isValidDuration(int minutes) {
    return minutes >= 1 && minutes <= 600;
}

bool Validator::isValidYear(int year) {
    return year >= 1 && year <= 8;
}