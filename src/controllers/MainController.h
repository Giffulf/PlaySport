#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include <string>

class MainController {
public:
    static std::string handleRequest(
        const std::string& method, 
        const std::string& path, 
        const std::string& body);
    
private:
    static std::string handleLogin(const std::string& body);
    static std::string handleRegister(const std::string& body);
    static std::string handleGetUsers();
    static std::string handleGetWorkouts();
    static std::string handleGetGoals();
    static std::string handleGetStats();
};

#endif // MAIN_CONTROLLER_H