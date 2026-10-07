#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"

class Admin : public User {
public:
    Admin(int id, string name,
          string username, string password);

    void displayRole() override;
};

#endif