#ifndef RESULT_H
#define RESULT_H

#include <string>

class Result {
private:
    int studentId;
    int examId;
    int score;
    int totalMarks;
    int correctCount;
    int attempted;
    int totalQuestions;
    std::string submittedAt;

public:
    static const int PASS_PERCENTAGE = 40;

    Result(int studentId, int examId, int score, int totalMarks,
           int correctCount, int attempted, int totalQuestions,
           const std::string& submittedAt);

    static std::string currentTimestamp();      // "YYYY-MM-DD HH:MM"

    int getStudentId() const;
    int getExamId() const;
    int getScore() const;
    int getTotalMarks() const;
    int getCorrectCount() const;
    int getAttempted() const;
    int getTotalQuestions() const;
    std::string getSubmittedAt() const;

    double getPercentage() const;
    bool isPassed() const;
    std::string getGrade() const;

    void display(const std::string& examTitle = "") const;
};

#endif
