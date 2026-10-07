#include "../../include/models/Admin.h"
#include <iostream>

using namespace std;

Admin::Admin(int id, string name, string username, string password)
    : User(id, name, username, password) {
}

void Admin::displayRole() {
    cout << "Role: Admin" << endl;
}
