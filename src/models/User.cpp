#include "../../include/models/User.h"

User::User(int id, string name, string username, string password) {
    this->id = id;
    this->name = name;
    this->username = username;
    this->password = password;
}

bool User::login(string username, string password) {
    return this->username == username &&
           this->password == password;
}

int User::getId() {
    return id;
}

string User::getName() {
    return name;
}

string User::getUsername() {
    return username;
}