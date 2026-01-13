#include "Config.h"
#include <fstream>
#include <iostream>

Config* Config::instance = nullptr;
std::mutex Config::mutex;

Config::Config() {
    settings["port"] = "8080";
    settings["static_dir"] = "public";
    settings["database_path"] = "playsport.db";
    settings["log_level"] = "info";
}

Config* Config::getInstance() {
    std::lock_guard<std::mutex> lock(mutex);
    if (!instance) {
        instance = new Config();
    }
    return instance;
}

void Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Using default configuration" << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
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
    return it != settings.end() ? it->second : defaultValue;
}

void Config::set(const std::string& key, const std::string& value) {
    settings[key] = value;
}

int Config::getPort() const {
    auto it = settings.find("port");
    return it != settings.end() ? std::stoi(it->second) : 8080;
}

std::string Config::getDatabasePath() const {
    Config* nonConstThis = const_cast<Config*>(this);
    return nonConstThis->get("database_path");
}