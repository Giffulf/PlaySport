#include "UserController.h"
#include <sstream>

std::string UserController::getUserStats(int userId) {
    std::stringstream response;
    response << "{";
    response << "\"success\":true,";
    response << "\"stats\":{";
    response << "\"activeWorkouts\":12,";
    response << "\"caloriesBurned\":3842,";
    response << "\"monthProgress\":75";
    response << "}";
    response << "}";
    return response.str();
}

std::string UserController::updateUserProfile(int userId, const std::string& userData) {
    std::stringstream response;
    response << "{";
    response << "\"success\":true,";
    response << "\"message\":\"Profile updated successfully\"";
    response << "}";
    return response.str();
}

std::string UserController::getUserGoals(int userId) {
    std::stringstream response;
    response << "{";
    response << "\"success\":true,";
    response << "\"goals\":[";
    response << "{\"id\":1,\"text\":\"Пробежать 5 км без остановки\",\"completed\":false},";
    response << "{\"id\":2,\"text\":\"30 отжиманий за подход\",\"completed\":true},";
    response << "{\"id\":3,\"text\":\"Сбросить 5 кг к декабрю\",\"completed\":false}";
    response << "]";
    response << "}";
    return response.str();
}