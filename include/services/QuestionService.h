#ifndef QUESTIONSERVICE_H
#define QUESTIONSERVICE_H

#include "../models/Question.h"
#include "../storage/QuestionRepository.h"
#include <memory>
#include <string>
#include <vector>

// Admin-side question management with validation.
class QuestionService {
private:
    QuestionRepository repo;
    std::string questionFile;

public:
    explicit QuestionService(const std::string& questionFile = "data/questions.txt");

    // On failure return false and put the reason in 'error'.
    bool addMCQ(int examId, const std::string& text, int marks,
                const std::vector<std::string>& options, char correct,
                std::string& error) const;
    bool addTrueFalse(int examId, const std::string& text, int marks,
                      bool correct, std::string& error) const;
    bool addDescriptive(int examId, const std::string& text, int marks, std::string& error) const;
    bool removeQuestion(int questionId, std::string& error) const;

    std::vector<std::unique_ptr<Question> > getQuestions(int examId) const;

    static int totalMarks(const std::vector<std::unique_ptr<Question> >& questions);
};

#endif
