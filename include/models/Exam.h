#ifndef EXAM_H
#define EXAM_H

#include <string>
using namespace std;

class Exam {
private:
    int examId;
    string title;
    int duration;
    int totalQuestions;

public:
    Exam(int id, string title, int duration, int totalQuestions);

    void displayExam();

    int getExamId();
    string getTitle();
    int getDuration();
    int getTotalQuestions();
};

#endif