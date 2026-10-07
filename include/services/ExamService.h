#ifndef EXAMSERVICE_H
#define EXAMSERVICE_H

#include "../models/Exam.h"

class ExamService {
public:
    void startExam(Exam exam);
    void displayExamDetails(Exam exam);
};

#endif