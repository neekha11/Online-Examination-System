#ifndef USER_H
#define USER_H

#include <string>

class User {
protected:
    int id;
    std::string name;
    std::string username;
    std::string passwordHash;   // "salt$hash" - the plain password is never stored

public:
    User(int id, const std::string& name,
         const std::string& username, const std::string& password);

    virtual ~User() = default;

    bool login(const std::string& username, const std::string& password) const;

    int getId() const;
    std::string getName() const;
    std::string getUsername() const;

    // Used by the repositories to save / restore the stored hash
    std::string getPasswordHash() const;
    void setPasswordHash(const std::string& hash);

    virtual void displayRole() const = 0;
};

#endif