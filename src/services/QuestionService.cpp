#include "../../include/services/QuestionService.h"
#include "../../include/models/DescriptiveQuestion.h"
#include "../../include/models/MCQQuestion.h"
#include "../../include/models/TrueFalseQuestion.h"
#include "../../include/utils/Validator.h"
#include <cctype>

using namespace std;

namespace {
bool isValidMarks(int marks) { return marks >= 1 && marks <= 100; }
}

QuestionService::QuestionService(const string& questionFile)
    : questionFile(questionFile) {
}

bool QuestionService::addMCQ(int examId, const string& text, int marks,
                             const vector<string>& options, char correct,
                             string& error) const {
    if (!Validator::isValidText(text, 300)) {
        error = "Question text must be 1-300 characters and cannot contain commas.";
        return false;
    }
    if (!isValidMarks(marks)) {
        error = "Marks must be between 1 and 100.";
        return false;
    }
    if (options.size() != 4) {
        error = "A multiple choice question needs exactly 4 options.";
        return false;
    }
    for (size_t i = 0; i < options.size(); ++i) {
        if (!Validator::isValidText(options[i], 150)) {
            error = "Each option must be 1-150 characters and cannot contain commas.";
            return false;
        }
    }
    const char c = static_cast<char>(toupper(static_cast<unsigned char>(correct)));
    if (c < 'A' || c > 'D') {
        error = "The correct option must be A, B, C or D.";
        return false;
    }

    MCQQuestion question(repo.nextId(questionFile), examId, text, marks, options, c);
    if (!repo.saveQuestion(question, questionFile)) {
        error = "Could not write to " + questionFile + ".";
        return false;
    }
    return true;
}

bool QuestionService::addTrueFalse(int examId, const string& text, int marks,
                                   bool correct, string& error) const {
    if (!Validator::isValidText(text, 300)) {
        error = "Question text must be 1-300 characters and cannot contain commas.";
        return false;
    }
    if (!isValidMarks(marks)) {
        error = "Marks must be between 1 and 100.";
        return false;
    }

    TrueFalseQuestion question(repo.nextId(questionFile), examId, text, marks, correct);
    if (!repo.saveQuestion(question, questionFile)) {
        error = "Could not write to " + questionFile + ".";
        return false;
    }
    return true;
}

bool QuestionService::addDescriptive(int examId, const string& text, int marks, string& error) const {
    if (!Validator::isValidText(text, 300)) {
        error = "Question text must be 1-300 characters and cannot contain commas.";
        return false;
    }
    if (!isValidMarks(marks)) {
        error = "Marks must be between 1 and 100.";
        return false;
    }

    DescriptiveQuestion question(repo.nextId(questionFile), examId, text, marks);
    if (!repo.saveQuestion(question, questionFile)) {
        error = "Could not write to " + questionFile + ".";
        return false;
    }
    return true;
}

bool QuestionService::removeQuestion(int questionId, string& error) const {
    if (!repo.deleteQuestion(questionId, questionFile)) {
        error = "No question with that ID was found.";
        return false;
    }
    return true;
}

vector<unique_ptr<Question> > QuestionService::getQuestions(int examId) const {
    return repo.findByExam(examId, questionFile);
}

int QuestionService::totalMarks(const vector<unique_ptr<Question> >& questions) {
    int total = 0;
    for (size_t i = 0; i < questions.size(); ++i)
        total += questions[i]->getMarks();
    return total;
}
