#include "AuthController.h"
#include "../models/User.h"
#include "../services/AuthService.h"
#include <sstream>

std::string AuthController::login(const std::string& email, const std::string& password) {
    User user = User::getByEmail(email);
    
    std::stringstream response;
    response << "{";
    
    if (user.verifyPassword(password)) {
        std::string token = AuthService::generateToken(user.getId());
        response << "\"success\":true,";
        response << "\"token\":\"" << token << "\",";
        response << "\"user\":" << user.toJson();
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
    
    // Проверка существования пользователя
    User existingUser = User::getByEmail(email);
    if (!existingUser.getEmail().empty()) {
        response << "\"success\":false,";
        response << "\"error\":\"User already exists\"";
    } else {
        User newUser;
        newUser.setName(name);
        newUser.setEmail(email);
        newUser.setPassword(password);
        
        if (newUser.save()) {
            std::string token = AuthService::generateToken(newUser.getId());
            response << "\"success\":true,";
            response << "\"token\":\"" << token << "\",";
            response << "\"user\":" << newUser.toJson();
        } else {
            response << "\"success\":false,";
            response << "\"error\":\"Registration failed\"";
        }
    }
    
    response << "}";
    return response.str();
}

std::string AuthController::logout(int userId) {
    AuthService::invalidateToken(userId);
    
    std::stringstream response;
    response << "{";
    response << "\"success\":true,";
    response << "\"message\":\"Logged out successfully\"";
    response << "}";
    return response.str();
}

std::string AuthController::getUserProfile(int userId) {
    User user = User::getById(userId);
    
    std::stringstream response;
    response << "{";
    
    if (user.getId() > 0) {
        response << "\"success\":true,";
        response << "\"user\":" << user.toJson();
    } else {
        response << "\"success\":false,";
        response << "\"error\":\"User not found\"";
    }
    
    response << "}";
    return response.str();
}