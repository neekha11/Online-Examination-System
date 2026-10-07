#include "../../include/services/ExamService.h"
#include <iostream>

using namespace std;

void ExamService::startExam(Exam exam) {
    cout << "Starting Exam..." << endl;
    exam.displayExam();
}

void ExamService::displayExamDetails(Exam exam) {
    exam.displayExam();
}