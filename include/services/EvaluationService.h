#ifndef EVALUATIONSERVICE_H
#define EVALUATIONSERVICE_H

#include "../models/AnswerSheet.h"
#include "../models/Exam.h"
#include "../models/Question.h"
#include "../models/Result.h"
#include <memory>
#include <vector>

// Automatic evaluation: 1 question = its marks if correct, 0 otherwise
// (no negative marking, unanswered questions score 0).
class EvaluationService {
public:
    Result evaluate(int studentId, const Exam& exam,
                    const std::vector<std::unique_ptr<Question> >& questions,
                    const AnswerSheet& answers) const;

    // Prints every question with the student's answer and the correct one.
    void displayReview(const std::vector<std::unique_ptr<Question> >& questions,
                       const AnswerSheet& answers) const;
};

#endif
