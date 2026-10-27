#ifndef ANSWERREPOSITORY_H
#define ANSWERREPOSITORY_H

#include <string>
#include <vector>

// A student's written answer plus the marks an admin gave it (-1 = not marked yet).
struct DescriptiveAnswer {
    int studentId;
    int examId;
    int questionId;
    int awarded;
    std::string text;
};

// File format: studentId,examId,questionId,awarded,answerText (commas/newlines escaped)
class AnswerRepository {
public:
    bool saveAnswers(const std::vector<DescriptiveAnswer>& answers, const std::string& filename) const;
    std::vector<DescriptiveAnswer> findBySubmission(int studentId, int examId,
                                                    const std::string& filename) const;

    // Stores the marks; 'previous' receives the old value (-1 if it was unmarked).
    // Returns false if that answer does not exist.
    bool setMarks(int studentId, int examId, int questionId, int awarded,
                  int& previous, const std::string& filename) const;
};

#endif
