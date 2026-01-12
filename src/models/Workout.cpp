#include "Workout.h"
#include <sstream>
#include <vector>

Workout::Workout() : id(0), duration(0), calories(0), userId(0) {}

bool Workout::save() {
    // В реальном проекте здесь была бы работа с БД
    return true;
}

bool Workout::update() {
    // В реальном проекте здесь была бы работа с БД
    return true;
}

bool Workout::remove() {
    // В реальном проекте здесь была бы работа с БД
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
    // Простой парсинг (для демо)
    // В реальном проекте используйте библиотеку JSON
}

std::vector<Workout> Workout::getUserWorkouts(int userId) {
    std::vector<Workout> workouts;
    
    // Демо-данные
    Workout w1;
    w1.setId(1);
    w1.setName("Утренняя пробежка");
    w1.setDescription("5 км по парку");
    w1.setDuration(30);
    w1.setCalories(300);
    w1.setType("running");
    w1.setUserId(userId);
    
    Workout w2;
    w2.setId(2);
    w2.setName("Силовая тренировка");
    w2.setDescription("Грудь и спина");
    w2.setDuration(45);
    w2.setCalories(350);
    w2.setType("strength");
    w2.setUserId(userId);
    
    workouts.push_back(w1);
    workouts.push_back(w2);
    
    return workouts;
}