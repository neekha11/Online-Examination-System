#include "../../include/models/Exam.h"
#include <iostream>

using namespace std;

Exam::Exam(int id, string title, int duration, int totalQuestions) {
    this->examId = id;
    this->title = title;
    this->duration = duration;
    this->totalQuestions = totalQuestions;
}

void Exam::displayExam() {
    cout << "Exam ID: " << examId << endl;
    cout << "Title: " << title << endl;
    cout << "Duration: " << duration << " minutes" << endl;
    cout << "Total Questions: " << totalQuestions << endl;
}

int Exam::getExamId() {
    return examId;
}

string Exam::getTitle() {
    return title;
}

int Exam::getDuration() {
    return duration;
}

int Exam::getTotalQuestions() {
    return totalQuestions;
}