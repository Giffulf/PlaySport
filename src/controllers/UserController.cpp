#include "UserController.h"
#include "../models/User.h"
#include "../models/Goal.h"
#include <sstream>

std::string UserController::getUserStats(int userId) {
    User user = User::getById(userId);
    
    std::stringstream response;
    response << "{";
    response << "\"success\":true,";
    response << "\"stats\":{";
    response << "\"activeWorkouts\":" << user.getActiveWorkouts() << ",";
    response << "\"totalCalories\":" << user.getTotalCalories() << ",";
    response << "\"monthProgress\":75";
    response << "}";
    response << "}";
    return response.str();
}

std::string UserController::updateUserProfile(int userId, const std::string& userData) {
    User user = User::getById(userId);
    user.fromJson(userData);
    
    std::stringstream response;
    response << "{";
    
    if (user.update()) {
        response << "\"success\":true,";
        response << "\"user\":" << user.toJson();
    } else {
        response << "\"success\":false,";
        response << "\"error\":\"Failed to update profile\"";
    }
    
    response << "}";
    return response.str();
}

std::string UserController::getUserGoals(int userId) {
    std::vector<Goal> goals = Goal::getUserGoals(userId);
    
    std::stringstream response;
    response << "{";
    response << "\"success\":true,";
    response << "\"goals\":[";
    
    for (size_t i = 0; i < goals.size(); i++) {
        response << goals[i].toJson();
        if (i < goals.size() - 1) {
            response << ",";
        }
    }
    
    response << "]";
    response << "}";
    return response.str();
}