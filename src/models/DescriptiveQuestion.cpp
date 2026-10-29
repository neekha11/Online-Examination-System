#include "../../include/models/DescriptiveQuestion.h"
#include "../../include/utils/StringUtils.h"
#include <iostream>

using namespace std;

namespace {
const size_t MAX_ANSWER_LENGTH = 2000;
}

DescriptiveQuestion::DescriptiveQuestion(int id, int examId, const string& text, int marks)
    : Question(id, examId, text, marks) {
}

string DescriptiveQuestion::normalizeAnswer(const string& input) const {
    string answer = StringUtils::trim(input);
    if (answer.size() > MAX_ANSWER_LENGTH)
        answer.resize(MAX_ANSWER_LENGTH);
    return answer;
}

string DescriptiveQuestion::getCorrectAnswer() const { return ""; }
string DescriptiveQuestion::getType() const { return "DESC"; }
string DescriptiveQuestion::answerPrompt() const { return "Your answer (Enter to skip): "; }
bool DescriptiveQuestion::isManuallyGraded() const { return true; }

void DescriptiveQuestion::display(int number) const {
    printHeader(number);
    cout << "   (Write your answer - it will be marked by your examiner)" << endl;
}

string DescriptiveQuestion::serialize() const {
    return to_string(id) + "," + to_string(examId) + ",DESC," + to_string(marks) + ",MANUAL," + text;
}
