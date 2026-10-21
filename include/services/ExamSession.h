#ifndef EXAMSESSION_H
#define EXAMSESSION_H

#include "../models/Exam.h"
#include "../models/Student.h"
#include <string>

// The complete student examination flow:
// exam details -> question paper -> timer -> answers -> evaluation -> result.
class ExamSession {
private:
    std::string questionFile;
    std::string resultFile;

public:
    ExamSession(const std::string& questionFile = "data/questions.txt",
                const std::string& resultFile = "data/results.txt");

    // Returns true if the exam was completed and the result saved.
    bool run(const Student& student, const Exam& exam) const;
};

#endif
