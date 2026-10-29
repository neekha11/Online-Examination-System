#include "../../include/storage/AnswerRepository.h"
#include "../../include/utils/FileUtils.h"
#include "../../include/utils/StringUtils.h"
#include <exception>

using namespace std;

namespace {

string toLine(const DescriptiveAnswer& a) {
    return to_string(a.studentId) + "," + to_string(a.examId) + "," + to_string(a.questionId) +
           "," + to_string(a.awarded) + "," + StringUtils::escapeField(a.text);
}

bool parse(const string& line, DescriptiveAnswer& a) {
    const vector<string> f = StringUtils::split(line);
    if (f.size() != 5)
        return false;
    try {
        a.studentId = stoi(f[0]);
        a.examId = stoi(f[1]);
        a.questionId = stoi(f[2]);
        a.awarded = stoi(f[3]);
    } catch (const exception&) {
        return false;
    }
    a.text = StringUtils::unescapeField(f[4]);
    return true;
}

} // namespace

bool AnswerRepository::saveAnswers(const vector<DescriptiveAnswer>& answers,
                                   const string& filename) const {
    for (size_t i = 0; i < answers.size(); ++i) {
        if (!FileUtils::appendLine(filename, toLine(answers[i])))
            return false;
    }
    return true;
}

vector<DescriptiveAnswer> AnswerRepository::findBySubmission(int studentId, int examId,
                                                             const string& filename) const {
    vector<DescriptiveAnswer> found;
    const vector<string> lines = FileUtils::readLines(filename);

    for (size_t i = 0; i < lines.size(); ++i) {
        DescriptiveAnswer a;
        if (parse(lines[i], a) && a.studentId == studentId && a.examId == examId)
            found.push_back(a);
    }
    return found;
}

bool AnswerRepository::setMarks(int studentId, int examId, int questionId, int awarded,
                                int& previous, const string& filename) const {
    vector<string> lines = FileUtils::readLines(filename);

    for (size_t i = 0; i < lines.size(); ++i) {
        DescriptiveAnswer a;
        if (parse(lines[i], a) && a.studentId == studentId && a.examId == examId &&
            a.questionId == questionId) {
            previous = a.awarded;
            a.awarded = awarded;
            lines[i] = toLine(a);
            return FileUtils::writeLines(filename, lines);
        }
    }
    return false;
}
