#include "../../include/models/Question.h"
#include <iostream>

using namespace std;

Question::Question(int id, int examId, const string& text, int marks)
    : id(id), examId(examId), text(text), marks(marks) {
}

void Question::printHeader(int number) const {
    cout << "Q" << number << ". " << text
         << "  [" << marks << (marks == 1 ? " mark]" : " marks]") << endl;
}

int Question::getId() const { return id; }
int Question::getExamId() const { return examId; }
string Question::getText() const { return text; }
int Question::getMarks() const { return marks; }

bool Question::isCorrect(const string& answer) const {
    const string normalized = normalizeAnswer(answer);
    return !normalized.empty() && normalized == getCorrectAnswer();
}
