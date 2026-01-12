#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

#include <string>

class AuthController {
public:
    // Обработка запросов аутентификации
    static std::string login(const std::string& email, const std::string& password);
    static std::string registerUser(const std::string& name, const std::string& email, 
                                   const std::string& password);
    static std::string logout(int userId);
    static std::string getUserProfile(int userId);
};

#endif // AUTH_CONTROLLER_H