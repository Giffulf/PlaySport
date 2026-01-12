#include "Config.h"
#include <fstream>
#include <iostream>
#include <sstream>

Config* Config::instance = nullptr;
std::mutex Config::mutex;

Config::Config() {
    // Значения по умолчанию
    settings["port"] = "8080";
    settings["database_path"] = "playsport.db";
    settings["jwt_secret"] = "play_sport_secret_key_2024";
    settings["log_level"] = "info";
}

Config* Config::getInstance() {
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr) {
        instance = new Config();
    }
    return instance;
}

void Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Config file not found, using defaults" << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            settings[key] = value;
        }
    }
}

std::string Config::get(const std::string& key, const std::string& defaultValue) {
    auto it = settings.find(key);
    if (it != settings.end()) {
        return it->second;
    }
    return defaultValue;
}

void Config::set(const std::string& key, const std::string& value) {
    settings[key] = value;
}

int Config::getPort() const {
    // Используем const_cast чтобы вызвать не-const метод
    const Config* self = this;
    return std::stoi(const_cast<Config*>(self)->get("port"));
}

std::string Config::getDatabasePath() const {
    const Config* self = this;
    return const_cast<Config*>(self)->get("database_path");
}