#include "../../include/models/QuestionPaper.h"
#include <iostream>

using namespace std;

QuestionPaper::QuestionPaper(int paperId, int examId,
                             const string& subject, int totalMarks)
    : paperId(paperId), examId(examId),
      subject(subject), totalMarks(totalMarks) {
}

void QuestionPaper::displayPaper() const {
    cout << "Paper ID: " << paperId << endl;
    cout << "Exam ID: " << examId << endl;
    cout << "Subject: " << subject << endl;
    cout << "Total Marks: " << totalMarks << endl;
}

int QuestionPaper::getPaperId() const {
    return paperId;
}

int QuestionPaper::getExamId() const {
    return examId;
}

string QuestionPaper::getSubject() const {
    return subject;
}

int QuestionPaper::getTotalMarks() const {
    return totalMarks;
}