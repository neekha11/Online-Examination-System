#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../models/User.h"
#include <string>
using namespace std;

class AuthService {
public:
    bool authenticate(User* user, string username, string password);
};

#endif