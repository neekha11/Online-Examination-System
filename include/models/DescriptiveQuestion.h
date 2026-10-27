#ifndef DESCRIPTIVEQUESTION_H
#define DESCRIPTIVEQUESTION_H

#include "Question.h"

// Free-text question marked by an admin.
// File format: id,examId,DESC,marks,MANUAL,text
class DescriptiveQuestion : public Question {
public:
    DescriptiveQuestion(int id, int examId, const std::string& text, int marks);

    std::string normalizeAnswer(const std::string& input) const override;   // trimmed, max 2000 chars
    std::string getCorrectAnswer() const override;                          // always empty
    std::string getType() const override;
    std::string answerPrompt() const override;
    void display(int number) const override;
    std::string serialize() const override;
    bool isManuallyGraded() const override;
};

#endif
