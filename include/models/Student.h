#ifndef STUDENT_H
#define STUDENT_H

#include "User.h"

class Student : public User {
private:
    string department;
    int year;

public:
    Student(int id, string name, string username,
            string password, string department, int year);

    void displayRole() override;

    string getDepartment();
    int getYear();
};

#endif