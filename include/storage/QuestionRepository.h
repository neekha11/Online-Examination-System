#ifndef QUESTIONREPOSITORY_H
#define QUESTIONREPOSITORY_H

#include "../models/Question.h"
#include <memory>
#include <string>
#include <vector>

// File format (one question per line):
//   MCQ: id,examId,MCQ,marks,correct(A-D),text,A,B,C,D
//   TF : id,examId,TF,marks,correct(TRUE|FALSE),text
class QuestionRepository {
public:
    // Returns false if the id already exists or the file cannot be written.
    bool saveQuestion(const Question& question, const std::string& filename) const;

    bool exists(int questionId, const std::string& filename) const;
    int nextId(const std::string& filename) const;

    std::vector<std::unique_ptr<Question> > loadAll(const std::string& filename) const;
    std::vector<std::unique_ptr<Question> > findByExam(int examId,
                                                       const std::string& filename) const;

    // Returns false if no question with this id exists.
    bool deleteQuestion(int questionId, const std::string& filename) const;
};

#endif
