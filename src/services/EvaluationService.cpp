#include "../../include/services/EvaluationService.h"
#include <iostream>

using namespace std;

Result EvaluationService::evaluate(int studentId, const Exam& exam,
                                   const vector<unique_ptr<Question> >& questions,
                                   const AnswerSheet& answers) const {
    int score = 0, totalMarks = 0, correct = 0, attempted = 0, pending = 0;

    for (size_t i = 0; i < questions.size(); ++i) {
        const Question& q = *questions[i];
        totalMarks += q.getMarks();

        if (!answers.hasAnswer(q.getId()))
            continue;

        ++attempted;
        if (q.isManuallyGraded()) {         // marked later by an admin
            ++pending;
            continue;
        }
        if (q.isCorrect(answers.getAnswer(q.getId()))) {
            ++correct;
            score += q.getMarks();
        }
    }

    return Result(studentId, exam.getExamId(), score, totalMarks, correct,
                  attempted, static_cast<int>(questions.size()),
                  Result::currentTimestamp(), pending);
}

void EvaluationService::displayReview(const vector<unique_ptr<Question> >& questions,
                                      const AnswerSheet& answers) const {
    for (size_t i = 0; i < questions.size(); ++i) {
        const Question& q = *questions[i];
        cout << endl;
        q.display(static_cast<int>(i) + 1);

        if (!answers.hasAnswer(q.getId())) {
            cout << "   Your answer: (not answered)" << endl;
        } else if (q.isManuallyGraded()) {
            cout << "   Your answer: " << answers.getAnswer(q.getId())
                 << "  -> Awaiting manual grading" << endl;
        } else {
            const bool ok = q.isCorrect(answers.getAnswer(q.getId()));
            cout << "   Your answer: " << answers.getAnswer(q.getId())
                 << (ok ? "  -> Correct" : "  -> Wrong") << endl;
        }
        if (!q.isManuallyGraded())
            cout << "   Correct answer: " << q.getCorrectAnswer() << endl;
    }
    cout << endl;
}
