#ifndef INPUT_H
#define INPUT_H

#include <string>

// Safe console input. If the input stream closes, the program exits cleanly.
class Input {
public:
    static std::string readLine(const std::string& prompt);      // trimmed
    static int readInt(const std::string& prompt, int min, int max);
    // Keeps asking until the text passes Validator::isValidText.
    static std::string readText(const std::string& prompt, std::size_t maxLength = 100);
    static bool confirm(const std::string& prompt);              // y / n
};

#endif
