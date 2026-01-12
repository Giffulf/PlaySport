#include "Workout.h"
#include <sstream>
#include <iostream>
#include <vector>

Workout::Workout() : id(0), duration(0), calories(0), userId(0) {}

std::vector<Workout> Workout::getUserWorkouts(int userId) {
    std::vector<Workout> workouts;
    
    // Создаем тестовые данные
    Workout workout1;
    workout1.setId(1);
    workout1.setName("Утренняя пробежка");
    workout1.setDescription("5 км по парку");
    workout1.setDuration(30);
    workout1.setCalories(300);
    workout1.setType("running");
    workout1.setUserId(userId);
    
    Workout workout2;
    workout2.setId(2);
    workout2.setName("Силовая тренировка");
    workout2.setDescription("Грудь и спина");
    workout2.setDuration(45);
    workout2.setCalories(350);
    workout2.setType("strength");
    workout2.setUserId(userId);
    
    workouts.push_back(workout1);
    workouts.push_back(workout2);
    
    return workouts;
}

bool Workout::save() {
    std::cout << "Saving workout: " << name << std::endl;
    return true;
}

bool Workout::update() {
    std::cout << "Updating workout: " << name << std::endl;
    return true;
}

bool Workout::remove() {
    std::cout << "Removing workout: " << name << std::endl;
    return true;
}

std::string Workout::toJson() const {
    std::stringstream json;
    json << "{";
    json << "\"id\":" << id << ",";
    json << "\"name\":\"" << name << "\",";
    json << "\"description\":\"" << description << "\",";
    json << "\"duration\":" << duration << ",";
    json << "\"calories\":" << calories << ",";
    json << "\"type\":\"" << type << "\",";
    json << "\"userId\":" << userId;
    json << "}";
    return json.str();
}

void Workout::fromJson(const std::string& json) {
    // Простой парсинг JSON
    if (json.find("\"name\":") != std::string::npos) {
        size_t start = json.find("\"name\":\"") + 8;
        size_t end = json.find("\"", start);
        if (end != std::string::npos) {
            name = json.substr(start, end - start);
        }
    }
    if (json.find("\"description\":") != std::string::npos) {
        size_t start = json.find("\"description\":\"") + 15;
        size_t end = json.find("\"", start);
        if (end != std::string::npos) {
            description = json.substr(start, end - start);
        }
    }
}