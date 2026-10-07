#include "../../include/models/QuestionPaper.h"
#include <iostream>

using namespace std;

QuestionPaper::QuestionPaper(int paperId, int examId,
                             string subject, int totalMarks) {
    this->paperId = paperId;
    this->examId = examId;
    this->subject = subject;
    this->totalMarks = totalMarks;
}

void QuestionPaper::displayPaper() {
    cout << "Paper ID: " << paperId << endl;
    cout << "Exam ID: " << examId << endl;
    cout << "Subject: " << subject << endl;
    cout << "Total Marks: " << totalMarks << endl;
}

int QuestionPaper::getPaperId() {
    return paperId;
}

int QuestionPaper::getExamId() {
    return examId;
}

string QuestionPaper::getSubject() {
    return subject;
}

int QuestionPaper::getTotalMarks() {
    return totalMarks;
}