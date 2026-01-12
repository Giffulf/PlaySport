#ifndef USER_CONTROLLER_H
#define USER_CONTROLLER_H

#include <string>

class UserController {
public:
    static std::string getUserStats(int userId);
    static std::string updateUserProfile(int userId, const std::string& userData);
    static std::string getUserGoals(int userId);
};

#endif // USER_CONTROLLER_H