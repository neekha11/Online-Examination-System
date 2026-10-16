#include "../../include/models/Admin.h"
#include <iostream>

using namespace std;

Admin::Admin(int id, const string& name,
             const string& username, const string& password)
    : User(id, name, username, password) {
}

void Admin::displayRole() const {
    cout << "Role: Admin" << endl;
}