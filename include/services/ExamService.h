#ifndef EXAMSERVICE_H
#define EXAMSERVICE_H

#include "../models/Exam.h"
#include "../models/Timer.h"

class ExamService {
public:
    void startExam(const Exam& exam) const;
    void displayExamDetails(const Exam& exam) const;

    // Creates a timer sized to the exam's duration (not started yet).
    Timer createTimer(const Exam& exam) const;
};

#endif