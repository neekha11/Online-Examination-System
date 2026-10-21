#include "../../include/models/Student.h"
#include <iostream>

using namespace std;

Student::Student(int id, const string& name, const string& username,
                 const string& password, const string& department, int year)
    : User(id, name, username, password),
      department(department), year(year) {
}

void Student::displayRole() const {
    cout << "Role: Student" << endl;
}

string Student::getDepartment() const {
    return department;
}

int Student::getYear() const {
    return year;
}