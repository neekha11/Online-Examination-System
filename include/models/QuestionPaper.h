#ifndef QUESTIONPAPER_H
#define QUESTIONPAPER_H

#include <string>

class QuestionPaper {
private:
    int paperId;
    int examId;
    std::string subject;
    int totalMarks;

public:
    QuestionPaper(int paperId, int examId,
                  const std::string& subject, int totalMarks);

    void displayPaper() const;

    int getPaperId() const;
    int getExamId() const;
    std::string getSubject() const;
    int getTotalMarks() const;
};

#endif