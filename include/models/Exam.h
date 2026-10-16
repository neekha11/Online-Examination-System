#ifndef EXAM_H
#define EXAM_H

#include <string>

class Exam {
private:
    int examId;
    std::string title;
    int duration;          // minutes
    int totalQuestions;

public:
    Exam(int id, const std::string& title, int duration, int totalQuestions);

    void displayExam() const;

    int getExamId() const;
    std::string getTitle() const;
    int getDuration() const;
    int getTotalQuestions() const;
};

#endif