#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace StringUtils {

inline std::string trim(const std::string& s) {
    std::size_t b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

inline std::string toUpper(std::string s) {
    for (std::size_t i = 0; i < s.size(); ++i)
        s[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(s[i])));
    return s;
}

inline std::vector<std::string> split(const std::string& line, char delim = ',') {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, delim))
        fields.push_back(field);
    return fields;
}

// Makes free text safe for the comma-separated data files.
inline std::string escapeField(const std::string& s) {
    std::string o;
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '%') o += "%25";
        else if (s[i] == ',') o += "%2C";
        else if (s[i] == '\n') o += "%0A";
        else if (s[i] != '\r') o += s[i];
    }
    return o;
}

inline std::string unescapeField(const std::string& s) {
    std::string o;
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '%' && i + 2 < s.size() + 0 && s.compare(i, 3, "%25") == 0) { o += '%'; i += 2; }
        else if (s[i] == '%' && i + 2 < s.size() + 0 && s.compare(i, 3, "%2C") == 0) { o += ','; i += 2; }
        else if (s[i] == '%' && i + 2 < s.size() + 0 && s.compare(i, 3, "%0A") == 0) { o += '\n'; i += 2; }
        else o += s[i];
    }
    return o;
}

} // namespace StringUtils

#endif
