#include "../../include/services/AuthService.h"
#include "../../include/utils/Validator.h"

using namespace std;

AuthService::AuthService(const string& studentFile)
    : studentFile(studentFile) {
}

bool AuthService::authenticate(const User* user, const string& username,
                               const string& password) const {
    if (user == nullptr)
        return false;

    return user->login(username, password);
}

bool AuthService::registerStudent(int id, const string& name,
                                  const string& username, const string& password,
                                  const string& department, int year,
                                  string& error) const {
    if (!Validator::isValidText(name)) {
        error = "Name must be 1-100 characters and cannot contain commas.";
        return false;
    }
    if (!Validator::isValidUsername(username)) {
        error = "Username must be 3-30 characters (letters, digits, underscore).";
        return false;
    }
    if (!Validator::isValidPassword(password)) {
        error = "Password must be at least 8 characters with a letter and a digit.";
        return false;
    }
    if (!Validator::isValidText(department)) {
        error = "Department must be 1-100 characters and cannot contain commas.";
        return false;
    }
    if (!Validator::isValidYear(year)) {
        error = "Year must be between 1 and 8.";
        return false;
    }
    if (studentRepo.existsById(id, studentFile)) {
        error = "A student with this ID already exists.";
        return false;
    }
    if (studentRepo.findByUsername(username, studentFile) != nullptr) {
        error = "This username is already taken.";
        return false;
    }

    Student student(id, name, username, password, department, year);

    if (!studentRepo.saveStudent(student, studentFile)) {
        error = "Could not write to " + studentFile + ".";
        return false;
    }

    return true;
}

unique_ptr<Student> AuthService::loginStudent(const string& username,
                                              const string& password) const {
    unique_ptr<Student> student = studentRepo.findByUsername(username, studentFile);

    if (!student || !student->login(username, password))
        return unique_ptr<Student>();

    return student;
}