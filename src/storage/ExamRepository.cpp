#include "../../include/storage/ExamRepository.h"
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

vector<Exam> ExamRepository::loadAll(const string& filename) const {
    vector<Exam> exams;

    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        const vector<string> f = splitLine(line);
        if (f.size() != 4)
            continue;                       // skip blank / damaged lines

        try {
            exams.push_back(Exam(stoi(f[0]), f[1], stoi(f[2]), stoi(f[3])));
        } catch (const exception&) {
            // skip lines with non-numeric values
        }
    }

    return exams;
}

bool ExamRepository::exists(int examId, const string& filename) const {
    return findById(examId, filename).has_value();
}

optional<Exam> ExamRepository::findById(int examId, const string& filename) const {
    for (const Exam& e : loadAll(filename)) {
        if (e.getExamId() == examId)
            return e;
    }
    return nullopt;
}

bool ExamRepository::saveExam(const Exam& exam, const string& filename) const {
    if (!Validator::isValidText(exam.getTitle()) ||
        !Validator::isValidDuration(exam.getDuration()) ||
        exam.getTotalQuestions() < 1)
        return false;

    if (exists(exam.getExamId(), filename))
        return false;

    const bool addNewline = needsLeadingNewline(filename);

    ofstream file(filename, ios::app);
    if (!file.is_open())
        return false;

    if (addNewline)
        file << '\n';

    file << exam.getExamId() << ','
         << exam.getTitle() << ','
         << exam.getDuration() << ','
         << exam.getTotalQuestions() << '\n';

    return file.good();
}