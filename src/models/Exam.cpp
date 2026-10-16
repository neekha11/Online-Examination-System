#include "../../include/models/Exam.h"
#include <iostream>

using namespace std;

Exam::Exam(int id, const string& title, int duration, int totalQuestions)
    : examId(id), title(title), duration(duration),
      totalQuestions(totalQuestions) {
}

void Exam::displayExam() const {
    cout << "Exam ID: " << examId << endl;
    cout << "Title: " << title << endl;
    cout << "Duration: " << duration << " minutes" << endl;
    cout << "Total Questions: " << totalQuestions << endl;
}

int Exam::getExamId() const {
    return examId;
}

string Exam::getTitle() const {
    return title;
}

int Exam::getDuration() const {
    return duration;
}

int Exam::getTotalQuestions() const {
    return totalQuestions;
}