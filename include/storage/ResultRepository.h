#ifndef RESULTREPOSITORY_H
#define RESULTREPOSITORY_H

#include "../models/Result.h"
#include <string>
#include <vector>

// File format (one result per line):
//   studentId,examId,score,totalMarks,correct,attempted,totalQuestions,submittedAt
class ResultRepository {
public:
    // Returns false if this student already has a result for the exam
    // or the file cannot be written.
    bool saveResult(const Result& result, const std::string& filename) const;

    // Replaces the stored result of the same student and exam.
    bool updateResult(const Result& result, const std::string& filename) const;

    bool hasAttempted(int studentId, int examId, const std::string& filename) const;

    std::vector<Result> loadAll(const std::string& filename) const;
    std::vector<Result> findByStudent(int studentId, const std::string& filename) const;
    std::vector<Result> findByExam(int examId, const std::string& filename) const;
};

#endif
