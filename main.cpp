#include <iostream>
#include "include/models/Student.h"
#include "include/models/Admin.h"
#include "include/models/Exam.h"
#include "include/models/QuestionPaper.h"
#include "include/models/Timer.h"
#include "include/storage/StudentRepository.h"
#include "include/storage/ExamRepository.h"
#include "include/services/AuthService.h"
#include "include/services/ExamService.h"

using namespace std;

int main() {

    Student student(
        101,
        "Kaushika",
        "kaushika",
        "1234",
        "CSE",
        2
    );

    Admin admin(
        1,
        "Administrator",
        "admin",
        "admin123"
    );

    cout << "Student Login: ";

    if (student.login("kaushika", "1234"))
        cout << "Successful\n";
    else
        cout << "Failed\n";

    cout << "Admin Login: ";

    if (admin.login("admin", "admin123"))
        cout << "Successful\n";
    else
        cout << "Failed\n";

    student.displayRole();
    admin.displayRole();

    Exam exam(101,
    "C++ OOP Exam",
    30,
    10
    );
    ExamService examService;
    examService.startExam(exam);

    exam.displayExam();

    QuestionPaper paper(1,
    101,
    "C++ OOP",
    50
    );

    paper.displayPaper();

    Timer timer(30);

    timer.displayTimer();

    timer.start();

    timer.displayTimer();

    timer.stop();

    timer.displayTimer();

    StudentRepository repository;
    repository.saveStudent(student, "data/students.txt");

    cout << "Student saved successfully." << endl;

    ExamRepository examRepository;
    examRepository.saveExam(exam, "data/exams.txt");
    cout << "Exam saved successfully." << endl;

    AuthService authService;

    if (authService.authenticate(&student, "kaushika", "1234"))
    cout << "Student authenticated successfully." << endl;
    else
    cout << "Student authentication failed." << endl;

    if (authService.authenticate(&admin, "admin", "admin123"))
    cout << "Admin authenticated successfully." << endl;
    else
    cout << "Admin authentication failed." << endl;
}