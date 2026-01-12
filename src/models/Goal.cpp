#include "Goal.h"
#include <sstream>
#include <iostream>
#include <vector>

Goal::Goal() : id(0), completed(false), userId(0) {}

std::vector<Goal> Goal::getUserGoals(int userId) {
    std::vector<Goal> goals;
    
    // Тестовые данные
    Goal goal1;
    goal1.setId(1);
    goal1.setText("Пробежать 5 км без остановки");
    goal1.setCompleted(false);
    goal1.setUserId(userId);
    
    Goal goal2;
    goal2.setId(2);
    goal2.setText("Сделать 50 отжиманий");
    goal2.setCompleted(true);
    goal2.setUserId(userId);
    
    goals.push_back(goal1);
    goals.push_back(goal2);
    
    return goals;
}

bool Goal::save() {
    std::cout << "Saving goal: " << text << std::endl;
    return true;
}

bool Goal::update() {
    std::cout << "Updating goal: " << text << std::endl;
    return true;
}

bool Goal::remove() {
    std::cout << "Removing goal: " << text << std::endl;
    return true;
}

void Goal::toggle() {
    completed = !completed;
}

std::string Goal::toJson() const {
    std::stringstream json;
    json << "{";
    json << "\"id\":" << id << ",";
    json << "\"text\":\"" << text << "\",";
    json << "\"completed\":" << (completed ? "true" : "false") << ",";
    json << "\"userId\":" << userId;
    json << "}";
    return json.str();
}

void Goal::fromJson(const std::string& json) {
    if (json.find("\"text\":") != std::string::npos) {
        size_t start = json.find("\"text\":\"") + 8;
        size_t end = json.find("\"", start);
        if (end != std::string::npos) {
            text = json.substr(start, end - start);
        }
    }
}