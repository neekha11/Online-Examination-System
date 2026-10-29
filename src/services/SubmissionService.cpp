#include "../../include/services/SubmissionService.h"
#include "../../include/services/EvaluationService.h"
#include "../../include/storage/AnswerRepository.h"
#include "../../include/storage/ResultRepository.h"

using namespace std;

SubmissionService::SubmissionService(const string& answerFile, const string& resultFile)
    : answerFile(answerFile), resultFile(resultFile) {
}

Result SubmissionService::submit(int studentId, const Exam& exam,
                                 const vector<unique_ptr<Question> >& questions,
                                 const AnswerSheet& answers) const {
    EvaluationService evaluator;
    const Result result = evaluator.evaluate(studentId, exam, questions, answers);

    if (!ResultRepository().saveResult(result, resultFile))
        return result;                      // already attempted: save nothing else

    vector<DescriptiveAnswer> written;
    for (size_t i = 0; i < questions.size(); ++i) {
        const Question& q = *questions[i];
        if (q.isManuallyGraded() && answers.hasAnswer(q.getId())) {
            DescriptiveAnswer a = {studentId, exam.getExamId(), q.getId(), -1, answers.getAnswer(q.getId())};
            written.push_back(a);
        }
    }
    if (!written.empty())
        AnswerRepository().saveAnswers(written, answerFile);

    return result;
}
