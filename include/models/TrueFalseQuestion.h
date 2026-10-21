#ifndef TRUEFALSEQUESTION_H
#define TRUEFALSEQUESTION_H

#include "Question.h"

// True / False question.
// File format: id,examId,TF,marks,TRUE|FALSE,text
class TrueFalseQuestion : public Question {
private:
    bool correct;

public:
    TrueFalseQuestion(int id, int examId, const std::string& text, int marks, bool correct);

    std::string normalizeAnswer(const std::string& input) const override;
    std::string getCorrectAnswer() const override;
    std::string getType() const override;
    std::string answerPrompt() const override;
    void display(int number) const override;
    std::string serialize() const override;
};

#endif
