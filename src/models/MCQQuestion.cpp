#include "../../include/models/MCQQuestion.h"
#include "../../include/utils/StringUtils.h"
#include <iostream>

using namespace std;

MCQQuestion::MCQQuestion(int id, int examId, const string& text, int marks,
                         const vector<string>& options, char correct)
    : Question(id, examId, text, marks), options(options),
      correct(static_cast<char>(toupper(static_cast<unsigned char>(correct)))) {
}

string MCQQuestion::normalizeAnswer(const string& input) const {
    const string a = StringUtils::toUpper(StringUtils::trim(input));
    if (a.size() == 1 && a[0] >= 'A' && a[0] < 'A' + static_cast<int>(options.size()))
        return a;
    return "";
}

string MCQQuestion::getCorrectAnswer() const { return string(1, correct); }
string MCQQuestion::getType() const { return "MCQ"; }
string MCQQuestion::answerPrompt() const { return "Your answer (A-D, Enter to skip): "; }

void MCQQuestion::display(int number) const {
    printHeader(number);
    for (size_t i = 0; i < options.size(); ++i)
        cout << "   " << static_cast<char>('A' + i) << ") " << options[i] << endl;
}

string MCQQuestion::serialize() const {
    string line = to_string(id) + "," + to_string(examId) + ",MCQ," +
                  to_string(marks) + "," + correct + "," + text;
    for (size_t i = 0; i < options.size(); ++i)
        line += "," + options[i];
    return line;
}
