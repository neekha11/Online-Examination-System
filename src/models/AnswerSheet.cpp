#include "../../include/models/AnswerSheet.h"

using namespace std;

void AnswerSheet::setAnswer(int questionId, const string& answer) {
    if (answer.empty())
        answers.erase(questionId);
    else
        answers[questionId] = answer;
}

bool AnswerSheet::hasAnswer(int questionId) const {
    return answers.find(questionId) != answers.end();
}

string AnswerSheet::getAnswer(int questionId) const {
    map<int, string>::const_iterator it = answers.find(questionId);
    return it == answers.end() ? string() : it->second;
}

size_t AnswerSheet::count() const {
    return answers.size();
}
