#include "../../include/storage/QuestionRepository.h"
#include "../../include/models/MCQQuestion.h"
#include "../../include/models/TrueFalseQuestion.h"
#include "../../include/utils/FileUtils.h"
#include "../../include/utils/StringUtils.h"
#include <algorithm>
#include <exception>

using namespace std;

namespace {

// Returns a null pointer for damaged or unknown lines.
unique_ptr<Question> parseQuestion(const string& line) {
    const vector<string> f = StringUtils::split(line);
    if (f.size() < 6)
        return unique_ptr<Question>();

    try {
        const int id = stoi(f[0]);
        const int examId = stoi(f[1]);
        const int marks = stoi(f[3]);

        if (f[2] == "MCQ" && f.size() == 10 && f[4].size() == 1) {
            vector<string> options(f.begin() + 6, f.end());
            return unique_ptr<Question>(
                new MCQQuestion(id, examId, f[5], marks, options, f[4][0]));
        }
        if (f[2] == "TF" && f.size() == 6 && (f[4] == "TRUE" || f[4] == "FALSE"))
            return unique_ptr<Question>(
                new TrueFalseQuestion(id, examId, f[5], marks, f[4] == "TRUE"));
    } catch (const exception&) {
    }
    return unique_ptr<Question>();
}

int idOfLine(const string& line) {
    try {
        return stoi(line.substr(0, line.find(',')));
    } catch (const exception&) {
        return -1;
    }
}

} // namespace

vector<unique_ptr<Question> > QuestionRepository::loadAll(const string& filename) const {
    vector<unique_ptr<Question> > questions;
    const vector<string> lines = FileUtils::readLines(filename);

    for (size_t i = 0; i < lines.size(); ++i) {
        unique_ptr<Question> q = parseQuestion(lines[i]);
        if (q)
            questions.push_back(move(q));
    }
    return questions;
}

vector<unique_ptr<Question> > QuestionRepository::findByExam(int examId,
                                                             const string& filename) const {
    vector<unique_ptr<Question> > all = loadAll(filename);
    vector<unique_ptr<Question> > result;

    for (size_t i = 0; i < all.size(); ++i) {
        if (all[i]->getExamId() == examId)
            result.push_back(move(all[i]));
    }
    return result;
}

bool QuestionRepository::exists(int questionId, const string& filename) const {
    const vector<string> lines = FileUtils::readLines(filename);
    for (size_t i = 0; i < lines.size(); ++i) {
        if (idOfLine(lines[i]) == questionId)
            return true;
    }
    return false;
}

int QuestionRepository::nextId(const string& filename) const {
    int highest = 0;
    const vector<string> lines = FileUtils::readLines(filename);
    for (size_t i = 0; i < lines.size(); ++i)
        highest = max(highest, idOfLine(lines[i]));
    return highest + 1;
}

bool QuestionRepository::saveQuestion(const Question& question,
                                      const string& filename) const {
    if (exists(question.getId(), filename))
        return false;
    return FileUtils::appendLine(filename, question.serialize());
}

bool QuestionRepository::deleteQuestion(int questionId, const string& filename) const {
    const vector<string> lines = FileUtils::readLines(filename);
    vector<string> kept;

    for (size_t i = 0; i < lines.size(); ++i) {
        if (idOfLine(lines[i]) != questionId)
            kept.push_back(lines[i]);
    }

    if (kept.size() == lines.size())
        return false;                       // nothing matched
    return FileUtils::writeLines(filename, kept);
}
