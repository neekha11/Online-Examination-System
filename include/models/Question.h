#ifndef QUESTION_H
#define QUESTION_H

#include <string>

// Abstract base class for every question type (MCQ, True/False, ...).
class Question {
protected:
    int id;
    int examId;
    std::string text;
    int marks;

    void printHeader(int number) const;   // "Q1. text  [1 mark]"

public:
    Question(int id, int examId, const std::string& text, int marks);
    virtual ~Question() = default;

    int getId() const;
    int getExamId() const;
    std::string getText() const;
    int getMarks() const;

    // Canonical form of the answer ("A".."D" or "TRUE"/"FALSE"),
    // or "" when the input is not a valid answer for this question.
    virtual std::string normalizeAnswer(const std::string& input) const = 0;
    virtual std::string getCorrectAnswer() const = 0;
    virtual std::string getType() const = 0;          // "MCQ" or "TF"
    virtual std::string answerPrompt() const = 0;
    virtual void display(int number) const = 0;
    virtual std::string serialize() const = 0;        // one line for questions.txt

    bool isCorrect(const std::string& answer) const;
};

#endif
