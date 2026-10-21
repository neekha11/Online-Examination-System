#include "../../include/services/ExamService.h"
#include <iostream>

using namespace std;

void ExamService::startExam(const Exam& exam) const {
    cout << "Starting Exam..." << endl;
    displayExamDetails(exam);
}

void ExamService::displayExamDetails(const Exam& exam) const {
    exam.displayExam();
}

Timer ExamService::createTimer(const Exam& exam) const {
    return Timer(exam.getDuration());
}