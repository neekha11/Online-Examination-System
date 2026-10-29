#include "../../include/storage/ResultRepository.h"
#include "../../include/utils/FileUtils.h"
#include "../../include/utils/StringUtils.h"
#include <exception>

using namespace std;

namespace {

string toLine(const Result& r) {
    return to_string(r.getStudentId()) + "," + to_string(r.getExamId()) + "," +
           to_string(r.getScore()) + "," + to_string(r.getTotalMarks()) + "," +
           to_string(r.getCorrectCount()) + "," + to_string(r.getAttempted()) + "," +
           to_string(r.getTotalQuestions()) + "," + r.getSubmittedAt() + "," +
           to_string(r.getPending());
}

} // namespace

// Older files have 8 fields (no "pending" column); they are still read.
vector<Result> ResultRepository::loadAll(const string& filename) const {
    vector<Result> results;
    const vector<string> lines = FileUtils::readLines(filename);

    for (size_t i = 0; i < lines.size(); ++i) {
        const vector<string> f = StringUtils::split(lines[i]);
        if (f.size() != 8 && f.size() != 9)
            continue;                       // skip damaged lines

        try {
            results.push_back(Result(stoi(f[0]), stoi(f[1]), stoi(f[2]), stoi(f[3]),
                                     stoi(f[4]), stoi(f[5]), stoi(f[6]), f[7],
                                     f.size() == 9 ? stoi(f[8]) : 0));
        } catch (const exception&) {
        }
    }
    return results;
}

bool ResultRepository::hasAttempted(int studentId, int examId,
                                    const string& filename) const {
    const vector<Result> all = loadAll(filename);
    for (size_t i = 0; i < all.size(); ++i) {
        if (all[i].getStudentId() == studentId && all[i].getExamId() == examId)
            return true;
    }
    return false;
}

bool ResultRepository::saveResult(const Result& r, const string& filename) const {
    if (hasAttempted(r.getStudentId(), r.getExamId(), filename))
        return false;
    return FileUtils::appendLine(filename, toLine(r));
}

bool ResultRepository::updateResult(const Result& r, const string& filename) const {
    vector<string> lines = FileUtils::readLines(filename);
    const string prefix = to_string(r.getStudentId()) + "," + to_string(r.getExamId()) + ",";
    bool found = false;

    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].compare(0, prefix.size(), prefix) == 0) {
            lines[i] = toLine(r);
            found = true;
            break;
        }
    }
    return found && FileUtils::writeLines(filename, lines);
}

vector<Result> ResultRepository::findByStudent(int studentId, const string& filename) const {
    vector<Result> all = loadAll(filename), found;
    for (size_t i = 0; i < all.size(); ++i) {
        if (all[i].getStudentId() == studentId)
            found.push_back(all[i]);
    }
    return found;
}

vector<Result> ResultRepository::findByExam(int examId, const string& filename) const {
    vector<Result> all = loadAll(filename), found;
    for (size_t i = 0; i < all.size(); ++i) {
        if (all[i].getExamId() == examId)
            found.push_back(all[i]);
    }
    return found;
}
