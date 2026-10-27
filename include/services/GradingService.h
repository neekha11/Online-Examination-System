#ifndef GRADINGSERVICE_H
#define GRADINGSERVICE_H

#include "../models/Result.h"
#include <string>

// Lets an admin give marks to a descriptive answer and updates the student's result.
class GradingService {
private:
    std::string questionFile;
    std::string answerFile;
    std::string resultFile;

public:
    GradingService(const std::string& questionFile = "data/questions.txt",
                   const std::string& answerFile = "data/answers.txt",
                   const std::string& resultFile = "data/results.txt");

    // Marks may be re-entered later; the score is adjusted by the difference.
    bool grade(int studentId, int examId, int questionId, int marks,
               std::string& error, Result* updated = nullptr) const;
};

#endif
