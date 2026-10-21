#include "../../include/models/TrueFalseQuestion.h"
#include "../../include/utils/StringUtils.h"
#include <iostream>

using namespace std;

TrueFalseQuestion::TrueFalseQuestion(int id, int examId, const string& text,
                                     int marks, bool correct)
    : Question(id, examId, text, marks), correct(correct) {
}

string TrueFalseQuestion::normalizeAnswer(const string& input) const {
    const string a = StringUtils::toUpper(StringUtils::trim(input));
    if (a == "T" || a == "TRUE") return "TRUE";
    if (a == "F" || a == "FALSE") return "FALSE";
    return "";
}

string TrueFalseQuestion::getCorrectAnswer() const { return correct ? "TRUE" : "FALSE"; }
string TrueFalseQuestion::getType() const { return "TF"; }
string TrueFalseQuestion::answerPrompt() const { return "Your answer (T/F, Enter to skip): "; }

void TrueFalseQuestion::display(int number) const {
    printHeader(number);
    cout << "   True or False?" << endl;
}

string TrueFalseQuestion::serialize() const {
    return to_string(id) + "," + to_string(examId) + ",TF," + to_string(marks) +
           "," + getCorrectAnswer() + "," + text;
}
