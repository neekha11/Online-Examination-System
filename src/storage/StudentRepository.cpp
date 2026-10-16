#include "../../include/storage/StudentRepository.h"
#include "../../include/utils/Validator.h"
#include <exception>
#include <fstream>
#include <sstream>

using namespace std;

namespace {

vector<string> splitLine(const string& line) {
    vector<string> fields;
    stringstream stream(line);
    string field;

    while (getline(stream, field, ','))
        fields.push_back(field);

    return fields;
}

// Old files may not end with a newline; without this a new record
// would be glued onto the last line.
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

vector<Student> StudentRepository::loadAll(const string& filename) const {
    vector<Student> students;

    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        const vector<string> f = splitLine(line);
        if (f.size() != 6)
            continue;                       // skip blank / damaged lines

        try {
            Student student(stoi(f[0]), f[1], f[2], "", f[4], stoi(f[5]));
            student.setPasswordHash(f[3]);
            students.push_back(student);
        } catch (const exception&) {
            // skip lines with a non-numeric id or year
        }
    }

    return students;
}

bool StudentRepository::existsById(int id, const string& filename) const {
    for (const Student& s : loadAll(filename)) {
        if (s.getId() == id)
            return true;
    }
    return false;
}

unique_ptr<Student> StudentRepository::findByUsername(const string& username,
                                                      const string& filename) const {
    const vector<Student> students = loadAll(filename);

    for (size_t i = 0; i < students.size(); ++i) {
        if (students[i].getUsername() == username)
            return unique_ptr<Student>(new Student(students[i]));
    }
    return unique_ptr<Student>();
}

bool StudentRepository::saveStudent(const Student& student,
                                    const string& filename) const {
    if (!Validator::isValidText(student.getName()) ||
        !Validator::isValidUsername(student.getUsername()) ||
        !Validator::isValidText(student.getDepartment()) ||
        !Validator::isValidYear(student.getYear()))
        return false;

    for (const Student& s : loadAll(filename)) {
        if (s.getId() == student.getId() ||
            s.getUsername() == student.getUsername())
            return false;
    }

    const bool addNewline = needsLeadingNewline(filename);

    ofstream file(filename, ios::app);
    if (!file.is_open())
        return false;

    if (addNewline)
        file << '\n';

    file << student.getId() << ','
         << student.getName() << ','
         << student.getUsername() << ','
         << student.getPasswordHash() << ','
         << student.getDepartment() << ','
         << student.getYear() << '\n';

    return file.good();
}