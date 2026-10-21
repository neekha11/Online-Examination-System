#include "../../include/utils/Input.h"
#include "../../include/utils/StringUtils.h"
#include "../../include/utils/Validator.h"
#include <cstdlib>
#include <exception>
#include <iostream>

using namespace std;

string Input::readLine(const string& prompt) {
    cout << prompt;
    cout.flush();

    string line;
    if (!getline(cin, line)) {
        cout << "\nInput closed. Exiting." << endl;
        exit(0);
    }
    return StringUtils::trim(line);
}

int Input::readInt(const string& prompt, int min, int max) {
    while (true) {
        const string text = readLine(prompt);
        try {
            size_t used = 0;
            const int value = stoi(text, &used);
            if (used == text.size() && value >= min && value <= max)
                return value;
        } catch (const exception&) {
        }
        cout << "Please enter a number between " << min << " and " << max << "." << endl;
    }
}

string Input::readText(const string& prompt, size_t maxLength) {
    while (true) {
        const string text = readLine(prompt);
        if (Validator::isValidText(text, maxLength))
            return text;
        cout << "Enter 1-" << maxLength << " characters (no commas)." << endl;
    }
}

bool Input::confirm(const string& prompt) {
    while (true) {
        const string answer = StringUtils::toUpper(readLine(prompt + " (y/n): "));
        if (answer == "Y" || answer == "YES") return true;
        if (answer == "N" || answer == "NO") return false;
    }
}
