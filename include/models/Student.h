#ifndef STUDENT_H
#define STUDENT_H

#include "User.h"

class Student : public User {
private:
    std::string department;
    int year;

public:
    Student(int id, const std::string& name, const std::string& username,
            const std::string& password, const std::string& department, int year);

    void displayRole() const override;

    std::string getDepartment() const;
    int getYear() const;
};

#endif