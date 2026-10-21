#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "include/models/Admin.h"
#include "include/models/Exam.h"
#include "include/models/Student.h"
#include "include/services/AuthService.h"
#include "include/services/ExamSession.h"
#include "include/services/QuestionService.h"
#include "include/storage/ExamRepository.h"
#include "include/storage/ResultRepository.h"
#include "include/storage/StudentRepository.h"
#include "include/utils/Input.h"
#include "include/utils/Validator.h"

using namespace std;

// Run this program from the project root so the data/ paths resolve.

namespace {

const string STUDENT_FILE  = "data/students.txt";
const string EXAM_FILE     = "data/exams.txt";
const string QUESTION_FILE = "data/questions.txt";
const string RESULT_FILE   = "data/results.txt";

// ---------- helpers ----------

void printExams(const vector<Exam>& exams) {
    cout << endl << left << setw(6) << "ID" << setw(34) << "Title"
         << setw(14) << "Duration" << "Questions" << endl;
    for (size_t i = 0; i < exams.size(); ++i) {
        cout << left << setw(6) << exams[i].getExamId()
             << setw(34) << exams[i].getTitle()
             << setw(14) << (to_string(exams[i].getDuration()) + " min")
             << exams[i].getTotalQuestions() << endl;
    }
    cout << endl;
}

// Lists the exams and lets the user pick one. Returns null if none chosen.
unique_ptr<Exam> chooseExam(const string& prompt) {
    ExamRepository repo;
    const vector<Exam> exams = repo.loadAll(EXAM_FILE);

    if (exams.empty()) {
        cout << "No exams available yet." << endl;
        return unique_ptr<Exam>();
    }

    printExams(exams);
    const int id = Input::readInt(prompt, 1, 1000000);
    unique_ptr<Exam> exam = repo.findById(id, EXAM_FILE);
    if (!exam)
        cout << "No exam with that ID." << endl;
    return exam;
}

string examTitle(int examId) {
    ExamRepository repo;
    unique_ptr<Exam> exam = repo.findById(examId, EXAM_FILE);
    return exam ? exam->getTitle() : "(deleted exam)";
}

// ---------- registration ----------

void registerStudent() {
    StudentRepository repo;
    const vector<Student> all = repo.loadAll(STUDENT_FILE);
    int id = 1;
    for (size_t i = 0; i < all.size(); ++i)
        id = max(id, all[i].getId() + 1);

    cout << "\n--- Student registration (your ID will be " << id << ") ---" << endl;
    const string name     = Input::readText("Full name: ");
    const string username = Input::readLine("Username (3-30 letters/digits/_): ");
    const string password = Input::readLine("Password (8+ chars, letter and digit): ");
    const string dept     = Input::readText("Department: ");
    const int year        = Input::readInt("Year (1-8): ", 1, 8);

    AuthService auth(STUDENT_FILE);
    string error;
    if (auth.registerStudent(id, name, username, password, dept, year, error))
        cout << "Registration successful. You can now log in." << endl;
    else
        cout << "Registration failed: " << error << endl;
}

// ---------- admin actions ----------

void createExam() {
    ExamRepository repo;
    const vector<Exam> all = repo.loadAll(EXAM_FILE);
    int id = 1;
    for (size_t i = 0; i < all.size(); ++i)
        id = max(id, all[i].getExamId() + 1);

    cout << "\n--- Create exam (ID " << id << ") ---" << endl;
    const string title = Input::readText("Title: ");
    int duration;
    do {
        duration = Input::readInt("Duration in minutes (1-600): ", 1, 600);
    } while (!Validator::isValidDuration(duration));
    const int questions = Input::readInt("Planned number of questions (1-200): ", 1, 200);

    if (repo.saveExam(Exam(id, title, duration, questions), EXAM_FILE))
        cout << "Exam created. Now add questions to it." << endl;
    else
        cout << "Exam could not be saved." << endl;
}

void addQuestion() {
    unique_ptr<Exam> exam = chooseExam("Add a question to exam ID: ");
    if (!exam)
        return;

    QuestionService service(QUESTION_FILE);
    string error;

    cout << "\nQuestion type: 1) Multiple choice   2) True/False" << endl;
    const int type = Input::readInt("Choose: ", 1, 2);
    const string text = Input::readText("Question text (no commas): ", 300);
    const int marks = Input::readInt("Marks (1-100): ", 1, 100);

    bool ok;
    if (type == 1) {
        vector<string> options;
        for (char c = 'A'; c <= 'D'; ++c)
            options.push_back(Input::readText(string("Option ") + c + ": ", 150));

        string letter;
        do {
            letter = Input::readLine("Correct option (A-D): ");
        } while (letter.size() != 1 || toupper(letter[0]) < 'A' || toupper(letter[0]) > 'D');

        ok = service.addMCQ(exam->getExamId(), text, marks, options, letter[0], error);
    } else {
        const bool correct = Input::confirm("Is the statement TRUE?");
        ok = service.addTrueFalse(exam->getExamId(), text, marks, correct, error);
    }

    cout << (ok ? "Question added." : "Could not add question: " + error) << endl;
}

void viewQuestions() {
    unique_ptr<Exam> exam = chooseExam("Show questions for exam ID: ");
    if (!exam)
        return;

    QuestionService service(QUESTION_FILE);
    vector<unique_ptr<Question> > questions = service.getQuestions(exam->getExamId());

    if (questions.empty()) {
        cout << "No questions yet for this exam." << endl;
        return;
    }

    cout << "\n" << exam->getTitle() << " - " << questions.size()
         << " question(s), " << QuestionService::totalMarks(questions) << " marks" << endl;
    for (size_t i = 0; i < questions.size(); ++i) {
        cout << "\n[Question ID " << questions[i]->getId() << "]" << endl;
        questions[i]->display(static_cast<int>(i) + 1);
        cout << "   Correct answer: " << questions[i]->getCorrectAnswer() << endl;
    }
}

void deleteQuestion() {
    const int id = Input::readInt("Question ID to delete (see 'View questions'): ", 1, 1000000);
    if (!Input::confirm("Delete question " + to_string(id) + "?"))
        return;

    QuestionService service(QUESTION_FILE);
    string error;
    cout << (service.removeQuestion(id, error) ? "Question deleted." : error) << endl;
}

void viewExamResults() {
    unique_ptr<Exam> exam = chooseExam("Show results for exam ID: ");
    if (!exam)
        return;

    ResultRepository results;
    const vector<Result> list = results.findByExam(exam->getExamId(), RESULT_FILE);
    if (list.empty()) {
        cout << "No one has taken this exam yet." << endl;
        return;
    }

    StudentRepository students;
    map<int, string> names;
    const vector<Student> all = students.loadAll(STUDENT_FILE);
    for (size_t i = 0; i < all.size(); ++i)
        names[all[i].getId()] = all[i].getName();

    cout << "\nResults for " << exam->getTitle() << endl;
    cout << left << setw(6) << "ID" << setw(22) << "Student" << setw(10) << "Score"
         << setw(9) << "%" << setw(7) << "Grade" << "Status" << endl;

    double total = 0;
    int passed = 0;
    for (size_t i = 0; i < list.size(); ++i) {
        const Result& r = list[i];
        const string name = names.count(r.getStudentId()) ? names[r.getStudentId()] : "(unknown)";
        ostringstream pct;
        pct << fixed << setprecision(1) << r.getPercentage() << "%";
        cout << left << setw(6) << r.getStudentId() << setw(22) << name
             << setw(10) << (to_string(r.getScore()) + "/" + to_string(r.getTotalMarks()))
             << setw(9) << pct.str() << setw(7) << r.getGrade()
             << (r.isPassed() ? "Passed" : "Failed") << endl;
        total += r.getPercentage();
        if (r.isPassed()) ++passed;
    }
    cout << "\nParticipants: " << list.size() << "   Passed: " << passed
         << "   Average: " << fixed << setprecision(1) << total / list.size() << "%" << endl;
}

void adminMenu(const Admin& admin) {
    cout << "\nWelcome, " << admin.getName() << "." << endl;
    while (true) {
        cout << "\n===== ADMIN MENU =====\n"
                "1) Create exam\n2) List exams\n3) Add question\n"
                "4) View questions\n5) Delete question\n6) View exam results\n0) Logout\n";
        switch (Input::readInt("Choose: ", 0, 6)) {
            case 1: createExam(); break;
            case 2: { ExamRepository r; const vector<Exam> e = r.loadAll(EXAM_FILE);
                      if (e.empty()) cout << "No exams yet." << endl; else printExams(e); break; }
            case 3: addQuestion(); break;
            case 4: viewQuestions(); break;
            case 5: deleteQuestion(); break;
            case 6: viewExamResults(); break;
            case 0: return;
        }
    }
}

// ---------- student actions ----------

void showMyResults(const Student& student) {
    ResultRepository repo;
    const vector<Result> results = repo.findByStudent(student.getId(), RESULT_FILE);
    if (results.empty()) {
        cout << "You have not taken any exams yet." << endl;
        return;
    }
    for (size_t i = 0; i < results.size(); ++i) {
        cout << endl;
        results[i].display(examTitle(results[i].getExamId()));
    }
}

void studentMenu(const Student& student) {
    cout << endl;
    student.displayRole();
    cout << "Welcome, " << student.getName() << "." << endl;

    while (true) {
        cout << "\n===== STUDENT MENU =====\n"
                "1) View available exams\n2) Take an exam\n3) My results\n0) Logout\n";
        switch (Input::readInt("Choose: ", 0, 3)) {
            case 1: { ExamRepository r; const vector<Exam> e = r.loadAll(EXAM_FILE);
                      if (e.empty()) cout << "No exams available yet." << endl; else printExams(e); break; }
            case 2: { unique_ptr<Exam> exam = chooseExam("Exam ID to take: ");
                      if (exam) ExamSession(QUESTION_FILE, RESULT_FILE).run(student, *exam);
                      break; }
            case 3: showMyResults(student); break;
            case 0: return;
        }
    }
}

} // namespace

int main() {
    AuthService auth(STUDENT_FILE);
    // Admin accounts are not stored in a file yet.
    Admin admin(1, "Administrator", "admin", "Admin@123");

    cout << "=====================================\n"
            "      ONLINE EXAMINATION SYSTEM\n"
            "=====================================" << endl;

    while (true) {
        cout << "\n1) Student login\n2) Admin login\n3) Register as student\n0) Exit\n";
        const int choice = Input::readInt("Choose: ", 0, 3);

        if (choice == 0) {
            cout << "Goodbye!" << endl;
            return 0;
        }
        if (choice == 3) {
            registerStudent();
            continue;
        }

        const string username = Input::readLine("Username: ");
        const string password = Input::readLine("Password: ");

        if (choice == 1) {
            unique_ptr<Student> student = auth.loginStudent(username, password);
            if (student) studentMenu(*student);
            else cout << "Login failed: wrong username or password." << endl;
        } else {
            if (auth.authenticate(&admin, username, password)) adminMenu(admin);
            else cout << "Login failed: wrong username or password." << endl;
        }
    }
}
