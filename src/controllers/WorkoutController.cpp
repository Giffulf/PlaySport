#include "WorkoutController.h"
#include "../models/Workout.h"
#include <sstream>

std::string WorkoutController::getUserWorkouts(int userId) {
    std::vector<Workout> workouts = Workout::getUserWorkouts(userId);
    
    std::stringstream response;
    response << "{";
    response << "\"success\":true,";
    response << "\"workouts\":[";
    
    for (size_t i = 0; i < workouts.size(); i++) {
        response << workouts[i].toJson();
        if (i < workouts.size() - 1) {
            response << ",";
        }
    }
    
    response << "]";
    response << "}";
    return response.str();
}

std::string WorkoutController::createWorkout(int userId, const std::string& workoutData) {
    Workout workout;
    workout.fromJson(workoutData);
    workout.setUserId(userId);
    
    std::stringstream response;
    response << "{";
    
    if (workout.save()) {
        response << "\"success\":true,";
        response << "\"workout\":" << workout.toJson();
    } else {
        response << "\"success\":false,";
        response << "\"error\":\"Failed to create workout\"";
    }
    
    response << "}";
    return response.str();
}

std::string WorkoutController::updateWorkout(int workoutId, const std::string& workoutData) {
    // В реальном проекте здесь была бы загрузка из БД
    Workout workout;
    workout.fromJson(workoutData);
    
    std::stringstream response;
    response << "{";
    
    if (workout.update()) {
        response << "\"success\":true,";
        response << "\"workout\":" << workout.toJson();
    } else {
        response << "\"success\":false,";
        response << "\"error\":\"Failed to update workout\"";
    }
    
    response << "}";
    return response.str();
}

std::string WorkoutController::deleteWorkout(int workoutId) {
    Workout workout;
    
    std::stringstream response;
    response << "{";
    
    if (workout.remove()) {
        response << "\"success\":true,";
        response << "\"message\":\"Workout deleted\"";
    } else {
        response << "\"success\":false,";
        response << "\"error\":\"Failed to delete workout\"";
    }
    
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