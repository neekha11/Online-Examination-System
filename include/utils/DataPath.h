#ifndef DATAPATH_H
#define DATAPATH_H

#include <cstdlib>
#include <string>

// Folder that holds the .txt data files ("data" by default).
// Set the DATA_DIR environment variable to use another folder, e.g. a persistent disk.
inline std::string dataDir() {
    const char* value = std::getenv("DATA_DIR");
    std::string dir = (value && *value) ? value : "data";
    while (dir.size() > 1 && (dir[dir.size() - 1] == '/' || dir[dir.size() - 1] == '\\'))
        dir.erase(dir.size() - 1);
    return dir;
}

inline std::string dataPath(const std::string& fileName) {
    return dataDir() + "/" + fileName;
}

#endif
