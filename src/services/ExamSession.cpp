#include "../../include/services/ExamSession.h"
#include "../../include/models/AnswerSheet.h"
#include "../../include/models/QuestionPaper.h"
#include "../../include/models/Timer.h"
#include "../../include/services/EvaluationService.h"
#include "../../include/services/ExamService.h"
#include "../../include/services/QuestionService.h"
#include "../../include/services/SubmissionService.h"
#include "../../include/storage/ResultRepository.h"
#include "../../include/utils/Input.h"
#include <iomanip>
#include <iostream>

using namespace std;

ExamSession::ExamSession(const string& questionFile, const string& resultFile, const string& answerFile)
    : questionFile(questionFile), resultFile(resultFile), answerFile(answerFile) {
}

bool ExamSession::run(const Student& student, const Exam& exam) const {
    ResultRepository resultRepo;

    if (resultRepo.hasAttempted(student.getId(), exam.getExamId(), resultFile)) {
        cout << "You have already attempted this exam." << endl;
        return false;
    }

    QuestionService questionService(questionFile);
    vector<unique_ptr<Question> > questions = questionService.getQuestions(exam.getExamId());

    if (questions.empty()) {
        cout << "No questions have been added to this exam yet." << endl;
        return false;
    }

    // ---- Exam details and question paper ----
    ExamService examService;
    cout << endl;
    examService.displayExamDetails(exam);

    QuestionPaper paper(exam.getExamId(), exam.getExamId(), exam.getTitle(),
                        QuestionService::totalMarks(questions));
    paper.displayPaper();
    cout << "Questions in paper: " << questions.size() << endl;
    cout << "Note: you can attempt this exam only once. Answers given after "
            "time runs out are not counted." << endl;

    if (!Input::confirm("Start the exam now?"))
        return false;

    // ---- Timed question loop ----
    Timer timer = examService.createTimer(exam);
    AnswerSheet answers;
    timer.start();

    for (size_t i = 0; i < questions.size(); ++i) {
        if (timer.isExpired()) {
            cout << "\nTime is up! Submitting your answers..." << endl;
            break;
        }

        const Question& q = *questions[i];
        const int left = timer.remainingSeconds();
        cout << "\nTime left: " << setfill('0') << setw(2) << left / 60 << ":"
             << setw(2) << left % 60 << setfill(' ') << endl;
        q.display(static_cast<int>(i) + 1);

        while (true) {
            const string input = Input::readLine(q.answerPrompt());
            if (input.empty())
                break;                                  // skipped

            const string answer = q.normalizeAnswer(input);
            if (answer.empty()) {
                cout << "Invalid answer. Try again." << endl;
                continue;
            }
            if (timer.isExpired()) {
                cout << "Time ran out before this answer was recorded." << endl;
                break;
            }
            answers.setAnswer(q.getId(), answer);
            break;
        }
    }
    timer.stop();

    // ---- Evaluation, storage and result ----
    EvaluationService evaluator;
    const Result result = SubmissionService(answerFile, resultFile)
                              .submit(student.getId(), exam, questions, answers);

    cout << endl;
    result.display(exam.getTitle());

    if (Input::confirm("Review your answers?"))
        evaluator.displayReview(questions, answers);

    return true;
}
