#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

class Validator {
public:
    // 3-30 characters: letters, digits and underscore only
    static bool isValidUsername(const std::string& username);

    // At least 8 characters, with at least one letter and one digit
    static bool isValidPassword(const std::string& password);

    // Non-blank text with no comma or line break (the data files are comma separated)
    static bool isValidText(const std::string& text, std::size_t maxLength = 100);

    // Exam duration in minutes (1 - 600)
    static bool isValidDuration(int minutes);

    // Academic year (1 - 8)
    static bool isValidYear(int year);
};

#endif