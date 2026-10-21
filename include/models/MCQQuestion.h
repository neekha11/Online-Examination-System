#ifndef MCQQUESTION_H
#define MCQQUESTION_H

#include "Question.h"
#include <vector>

// Multiple choice question with four options (A-D).
// File format: id,examId,MCQ,marks,correct,text,A,B,C,D
class MCQQuestion : public Question {
private:
    std::vector<std::string> options;
    char correct;                       // 'A' - 'D'

public:
    MCQQuestion(int id, int examId, const std::string& text, int marks,
                const std::vector<std::string>& options, char correct);

    std::string normalizeAnswer(const std::string& input) const override;
    std::string getCorrectAnswer() const override;
    std::string getType() const override;
    std::string answerPrompt() const override;
    void display(int number) const override;
    std::string serialize() const override;
    std::vector<std::string> getOptions() const override;
};

#endif
