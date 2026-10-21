#include "../../include/utils/FileUtils.h"
#include <fstream>

using namespace std;

namespace {

bool needsLeadingNewline(const string& filename) {
    ifstream in(filename, ios::binary | ios::ate);
    if (!in.is_open() || in.tellg() <= 0)
        return false;

    in.seekg(-1, ios::end);
    char last = '\n';
    in.get(last);
    return last != '\n';
}

} // namespace

bool FileUtils::appendLine(const string& filename, const string& line) {
    const bool addNewline = needsLeadingNewline(filename);

    ofstream file(filename, ios::app);
    if (!file.is_open())
        return false;

    if (addNewline)
        file << '\n';
    file << line << '\n';
    return file.good();
}

vector<string> FileUtils::readLines(const string& filename) {
    vector<string> lines;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (!line.empty())
            lines.push_back(line);
    }
    return lines;
}

bool FileUtils::writeLines(const string& filename, const vector<string>& lines) {
    ofstream file(filename, ios::trunc);
    if (!file.is_open())
        return false;

    for (size_t i = 0; i < lines.size(); ++i)
        file << lines[i] << '\n';
    return file.good();
}
