#include "../../include/services/AuthService.h"

bool AuthService::authenticate(User* user,
                               string username,
                               string password) {

    return user->login(username, password);
}