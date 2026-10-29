#include "../../include/services/GradingService.h"
#include "../../include/services/QuestionService.h"
#include "../../include/storage/AnswerRepository.h"
#include "../../include/storage/ResultRepository.h"
#include <algorithm>

using namespace std;

GradingService::GradingService(const string& questionFile, const string& answerFile,
                               const string& resultFile)
    : questionFile(questionFile), answerFile(answerFile), resultFile(resultFile) {
}

bool GradingService::grade(int studentId, int examId, int questionId, int marks,
                           string& error, Result* updated) const {
    // 1. The question must be a descriptive question of this exam.
    vector<unique_ptr<Question> > questions = QuestionService(questionFile).getQuestions(examId);
    int maxMarks = -1;
    for (size_t i = 0; i < questions.size(); ++i) {
        if (questions[i]->getId() == questionId && questions[i]->isManuallyGraded())
            maxMarks = questions[i]->getMarks();
    }
    if (maxMarks < 0) {
        error = "That is not a descriptive question of this exam.";
        return false;
    }
    if (marks < 0 || marks > maxMarks) {
        error = "Marks must be between 0 and " + to_string(maxMarks) + ".";
        return false;
    }

    // 2. The student's result must exist.
    ResultRepository results;
    const vector<Result> mine = results.findByStudent(studentId, resultFile);
    const Result* current = nullptr;
    for (size_t i = 0; i < mine.size(); ++i) {
        if (mine[i].getExamId() == examId)
            current = &mine[i];
    }
    if (!current) {
        error = "This student has no result for that exam.";
        return false;
    }

    // 3. Save the marks, then adjust the stored result.
    int previous = -1;
    if (!AnswerRepository().setMarks(studentId, examId, questionId, marks, previous, answerFile)) {
        error = "No written answer was found for that question.";
        return false;
    }

    const int score = current->getScore() + marks - max(previous, 0);
    const int pending = max(0, current->getPending() - (previous < 0 ? 1 : 0));
    const Result fresh(studentId, examId, score, current->getTotalMarks(), current->getCorrectCount(),
                       current->getAttempted(), current->getTotalQuestions(),
                       current->getSubmittedAt(), pending);

    if (!results.updateResult(fresh, resultFile)) {
        error = "The result could not be updated.";
        return false;
    }
    if (updated)
        *updated = fresh;
    return true;
}
