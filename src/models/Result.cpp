#include "../../include/models/Result.h"
#include <ctime>
#include <iomanip>
#include <iostream>

using namespace std;

Result::Result(int studentId, int examId, int score, int totalMarks,
               int correctCount, int attempted, int totalQuestions,
               const string& submittedAt, int pending)
    : studentId(studentId), examId(examId), score(score), totalMarks(totalMarks),
      correctCount(correctCount), attempted(attempted),
      totalQuestions(totalQuestions), submittedAt(submittedAt), pending(pending) {
}

string Result::currentTimestamp() {
    const time_t now = time(nullptr);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", localtime(&now));
    return buffer;
}

int Result::getStudentId() const { return studentId; }
int Result::getExamId() const { return examId; }
int Result::getScore() const { return score; }
int Result::getTotalMarks() const { return totalMarks; }
int Result::getCorrectCount() const { return correctCount; }
int Result::getAttempted() const { return attempted; }
int Result::getTotalQuestions() const { return totalQuestions; }
string Result::getSubmittedAt() const { return submittedAt; }
int Result::getPending() const { return pending; }

double Result::getPercentage() const {
    return totalMarks > 0 ? 100.0 * score / totalMarks : 0.0;
}

bool Result::isPassed() const {
    return getPercentage() >= PASS_PERCENTAGE;
}

string Result::getGrade() const {
    const double p = getPercentage();
    if (p >= 90) return "A+";
    if (p >= 80) return "A";
    if (p >= 70) return "B";
    if (p >= 60) return "C";
    if (p >= 50) return "D";
    if (p >= PASS_PERCENTAGE) return "E";
    return "F";
}

void Result::display(const string& examTitle) const {
    cout << "------------- RESULT -------------" << endl;
    if (!examTitle.empty())
        cout << "Exam:        " << examTitle << endl;
    cout << "Exam ID:     " << examId << endl;
    cout << "Score:       " << score << " / " << totalMarks << endl;
    cout << "Percentage:  " << fixed << setprecision(1) << getPercentage() << "%" << endl;
    cout << "Grade:       " << getGrade() << endl;
    cout << "Correct:     " << correctCount << " of " << totalQuestions << endl;
    cout << "Attempted:   " << attempted << " of " << totalQuestions << endl;
    cout << "Status:      " << (isPassed() ? "PASSED" : "FAILED") << endl;
    if (pending > 0)
        cout << "Pending:     " << pending << " answer(s) awaiting manual grading" << endl;
    cout << "Submitted:   " << submittedAt << endl;
    cout << "----------------------------------" << endl;
}
