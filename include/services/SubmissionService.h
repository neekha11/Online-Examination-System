#ifndef SUBMISSIONSERVICE_H
#define SUBMISSIONSERVICE_H

#include "../models/AnswerSheet.h"
#include "../models/Exam.h"
#include "../models/Question.h"
#include "../models/Result.h"
#include <memory>
#include <string>
#include <vector>

// Evaluates an attempt, then saves the result and any descriptive answers.
class SubmissionService {
private:
    std::string answerFile;
    std::string resultFile;

public:
    SubmissionService(const std::string& answerFile = "data/answers.txt",
                      const std::string& resultFile = "data/results.txt");

    Result submit(int studentId, const Exam& exam,
                  const std::vector<std::unique_ptr<Question> >& questions,
                  const AnswerSheet& answers) const;
};

#endif
