#ifndef QUESTIONPAPER_H
#define QUESTIONPAPER_H

#include <string>
using namespace std;

class QuestionPaper {
private:
    int paperId;
    int examId;
    string subject;
    int totalMarks;

public:
    QuestionPaper(int paperId, int examId,
                  string subject, int totalMarks);

    void displayPaper();

    int getPaperId();
    int getExamId();
    string getSubject();
    int getTotalMarks();
};

#endif