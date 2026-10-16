#include <iostream>
#include <memory>
#include <string>
#include "include/models/Student.h"
#include "include/models/Admin.h"
#include "include/models/Exam.h"
#include "include/models/QuestionPaper.h"
#include "include/models/Timer.h"
#include "include/storage/ExamRepository.h"
#include "include/services/AuthService.h"
#include "include/services/ExamService.h"

using namespace std;

// Run this program from the project root so the data/ paths resolve.
int main() {
    const string studentFile = "data/students.txt";
    const string examFile = "data/exams.txt";

    // ---------- Students: register once, then log in from the data file ----------
    AuthService authService(studentFile);
    string error;

    if (authService.registerStudent(101, "Kaushika", "kaushika", "Exam@1234",
                                    "CSE", 2, error))
        cout << "Demo student registered." << endl;
    else
        cout << "Registration skipped: " << error << endl;

    unique_ptr<Student> student = authService.loginStudent("kaushika", "Exam@1234");
    cout << "Student Login: " << (student ? "Successful" : "Failed") << endl;

    unique_ptr<Student> wrong = authService.loginStudent("kaushika", "wrongpass1");
    cout << "Student Login (wrong password): "
         << (wrong ? "Successful" : "Failed") << endl;

    if (student)
        student->displayRole();

    // ---------- Admin (not stored in a file yet) ----------
    Admin admin(1, "Administrator", "admin", "Admin@123");

    cout << "Admin Login: "
         << (authService.authenticate(&admin, "admin", "Admin@123")
                 ? "Successful" : "Failed")
         << endl;
    admin.displayRole();

    // ---------- Exam: save once, then load it back from the file ----------
    ExamRepository examRepository;
    Exam newExam(101, "C++ OOP Exam", 30, 10);

    if (examRepository.saveExam(newExam, examFile))
        cout << "Exam saved successfully." << endl;
    else
        cout << "Exam not saved (already exists or invalid)." << endl;

    unique_ptr<Exam> exam = examRepository.findById(101, examFile);
    if (!exam) {
        cerr << "Exam 101 could not be loaded from " << examFile << endl;
        return 1;
    }

    ExamService examService;
    examService.startExam(*exam);

    QuestionPaper paper(1, exam->getExamId(), "C++ OOP", 50);
    paper.displayPaper();

    // ---------- Timer ----------
    Timer timer = examService.createTimer(*exam);

    timer.displayTimer();
    timer.start();
    timer.displayTimer();
    timer.stop();
    timer.displayTimer();

    return 0;
}