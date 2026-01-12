#include "AuthController.h"
#include <sstream>

std::string AuthController::login(const std::string& email, const std::string& password) {
    std::stringstream response;
    response << "{";
    
    // Демо-авторизация
    if (email == "test@example.com" && password == "password") {
        response << "\"success\":true,";
        response << "\"token\":\"ps_token_123456\",";
        response << "\"user\":{";
        response << "\"id\":1,";
        response << "\"name\":\"Иван Петров\",";
        response << "\"email\":\"test@example.com\",";
        response << "\"activeWorkouts\":5,";
        response << "\"totalCalories\":2450";
        response << "}";
    } else {
        response << "\"success\":false,";
        response << "\"error\":\"Invalid credentials\"";
    }
    
    response << "}";
    return response.str();
}

std::string AuthController::registerUser(const std::string& name, const std::string& email,
                                        const std::string& password) {
    std::stringstream response;
    response << "{";
    
    // Демо-регистрация
    if (!email.empty() && !password.empty()) {
        response << "\"success\":true,";
        response << "\"message\":\"Registration successful\",";
        response << "\"userId\":100,";
        response << "\"email\":\"" << email << "\"";
    } else {
        response << "\"success\":false,";
        response << "\"error\":\"All fields are required\"";
    }
    
    response << "}";
    return response.str();
}

std::string AuthController::logout(int userId) {
    std::stringstream response;
    response << "{";
    response << "\"success\":true,";
    response << "\"message\":\"Logged out successfully\"";
    response << "}";
    return response.str();
}

std::string AuthController::getUserProfile(int userId) {
    std::stringstream response;
    response << "{";
    response << "\"success\":true,";
    response << "\"user\":{";
    response << "\"id\":" << userId << ",";
    response << "\"name\":\"Иван Петров\",";
    response << "\"email\":\"test@example.com\",";
    response << "\"activeWorkouts\":5,";
    response << "\"totalCalories\":2450";
    response << "}";
    response << "}";
    return response.str();
}