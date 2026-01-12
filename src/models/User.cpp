#include "User.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <functional> // для std::hash

User::User() : id(0), activeWorkouts(0), totalCalories(0) {
    time_t now = time(0);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    createdAt = buffer;
}

std::vector<User> User::getAll() {
    std::vector<User> users;
    return users;
}

User User::getById(int userId) {
    User user;
    return user;
}

User User::getByEmail(const std::string& email) {
    User user;
    user.setEmail(email);
    return user;
}

bool User::save() {
    std::cout << "Saving user: " << name << std::endl;
    return true;
}

bool User::update() {
    std::cout << "Updating user: " << name << std::endl;
    return true;
}

bool User::remove() {
    std::cout << "Removing user: " << name << std::endl;
    return true;
}

void User::setPassword(const std::string& password) {
    passwordHash = hashPassword(password);
}

std::string User::hashPassword(const std::string& password) const {
    // Простая хэш-функция без OpenSSL
    std::hash<std::string> hasher;
    size_t hash = hasher(password + "play_sport_salt");
    
    std::stringstream ss;
    ss << std::hex << hash;
    return ss.str();
}

bool User::verifyPassword(const std::string& password) const {
    return hashPassword(password) == passwordHash;
}

std::string User::toJson() const {
    std::stringstream json;
    json << "{";
    json << "\"id\":" << id << ",";
    json << "\"name\":\"" << name << "\",";
    json << "\"email\":\"" << email << "\",";
    json << "\"createdAt\":\"" << createdAt << "\",";
    json << "\"activeWorkouts\":" << activeWorkouts << ",";
    json << "\"totalCalories\":" << totalCalories;
    json << "}";
    return json.str();
}

void User::fromJson(const std::string& json) {
    if (json.find("\"name\":") != std::string::npos) {
        size_t start = json.find("\"name\":\"") + 8;
        size_t end = json.find("\"", start);
        name = json.substr(start, end - start);
    }
}