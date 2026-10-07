#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

class User {
protected:
    int id;
    string name;
    string username;
    string password;

public:
    User(int id, string name, string username, string password);

    bool login(string username, string password);

    int getId();

    virtual void displayRole() = 0;

    virtual ~User() {}
    string getName();
    string getUsername();
};

#endif