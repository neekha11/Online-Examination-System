#ifndef EXAMREPOSITORY_H
#define EXAMREPOSITORY_H

#include "../models/Exam.h"
#include <optional>
#include <string>
#include <vector>

// File format (one exam per line):
//   id,title,durationMinutes,totalQuestions
class ExamRepository {
public:
    // Returns false if the data is invalid, the id already exists,
    // or the file cannot be written.
    bool saveExam(const Exam& exam, const std::string& filename) const;

    bool exists(int examId, const std::string& filename) const;
    std::optional<Exam> findById(int examId, const std::string& filename) const;
    std::vector<Exam> loadAll(const std::string& filename) const;
};

#endif