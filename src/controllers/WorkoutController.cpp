#include "WorkoutController.h"
#include <sstream>

std::string WorkoutController::getUserWorkouts(int userId) {
    std::stringstream response;
    response << "{";
    response << "\"success\":true,";
    response << "\"workouts\":[";
    response << "{\"id\":1,\"name\":\"Утренняя пробежка\",\"duration\":30,\"calories\":300,\"type\":\"running\"},";
    response << "{\"id\":2,\"name\":\"Силовая тренировка\",\"duration\":45,\"calories\":350,\"type\":\"strength\"},";
    response << "{\"id\":3,\"name\":\"Велосипед\",\"duration\":60,\"calories\":480,\"type\":\"cycling\"}";
    response << "]";
    response << "}";
    return response.str();
}

std::string WorkoutController::createWorkout(int userId, const std::string& workoutData) {
    std::stringstream response;
    response << "{";
    response << "\"success\":true,";
    response << "\"message\":\"Workout created successfully\"";
    response << "}";
    return response.str();
}

std::string WorkoutController::updateWorkout(int workoutId, const std::string& workoutData) {
    std::stringstream response;
    response << "{";
    response << "\"success\":true,";
    response << "\"message\":\"Workout updated successfully\"";
    response << "}";
    return response.str();
}

std::string WorkoutController::deleteWorkout(int workoutId) {
    std::stringstream response;
    response << "{";
    response << "\"success\":true,";
    response << "\"message\":\"Workout deleted\"";
    response << "}";
    return response.str();
}

std::string WorkoutController::startWorkout(int workoutId) {
    std::stringstream response;
    response << "{";
    response << "\"success\":true,";
    response << "\"message\":\"Workout started\"," ;
    response << "\"workoutId\":" << workoutId;
    response << "}";
    return response.str();
}