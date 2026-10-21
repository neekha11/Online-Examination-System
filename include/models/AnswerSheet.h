#ifndef ANSWERSHEET_H
#define ANSWERSHEET_H

#include <cstddef>
#include <map>
#include <string>

// Holds one student's answers for one exam attempt (questionId -> answer).
class AnswerSheet {
private:
    std::map<int, std::string> answers;

public:
    // An empty answer removes any stored answer (question left unanswered).
    void setAnswer(int questionId, const std::string& answer);
    bool hasAnswer(int questionId) const;
    std::string getAnswer(int questionId) const;
    std::size_t count() const;
};

#endif
