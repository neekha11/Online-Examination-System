#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../models/User.h"
#include "../models/Student.h"
#include "../storage/StudentRepository.h"
#include <memory>
#include <string>

class AuthService {
private:
    StudentRepository studentRepo;
    std::string studentFile;

public:
    explicit AuthService(const std::string& studentFile = "data/students.txt");

    // Checks credentials against an existing User object (null-safe).
    bool authenticate(const User* user, const std::string& username,
                      const std::string& password) const;

    // Validates the input, then stores a new student.
    // On failure returns false and puts the reason in 'error'.
    bool registerStudent(int id, const std::string& name,
                         const std::string& username, const std::string& password,
                         const std::string& department, int year,
                         std::string& error) const;

    // Looks the student up in the data file and checks the password.
    // Returns the student on success, or a null pointer on failure.
    std::unique_ptr<Student> loginStudent(const std::string& username,
                                          const std::string& password) const;
};

#endif