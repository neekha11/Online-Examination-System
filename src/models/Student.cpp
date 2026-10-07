#include "../../include/models/Student.h"
#include <iostream>
using namespace std;

Student::Student(int id, string name, string username,
                 string password, string department, int year)
    : User(id, name, username, password) {

    this->department = department;
    this->year = year;
}

void Student::displayRole() {
    cout << "Role: Student" << endl;
}
string Student::getDepartment() {
    return department;
}

int Student::getYear() {
    return year;
}