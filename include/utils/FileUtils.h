#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <vector>

class FileUtils {
public:
    // Adds one line to the end of the file (creates the file if needed).
    static bool appendLine(const std::string& filename, const std::string& line);

    // Reads all non-blank lines (carriage returns removed).
    static std::vector<std::string> readLines(const std::string& filename);

    // Replaces the whole file with the given lines.
    static bool writeLines(const std::string& filename,
                           const std::vector<std::string>& lines);
};

#endif
