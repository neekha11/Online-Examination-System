#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"

class Admin : public User {
public:
    Admin(int id, const std::string& name,
          const std::string& username, const std::string& password);

    void displayRole() const override;
};

#endif