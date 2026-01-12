#include "DataAdapter.h"
#include <sstream>
#include <algorithm>

std::map<std::string, std::string> DataAdapter::jsonToMap(const std::string& json) {
    std::map<std::string, std::string> result;
    
    // Простой парсинг JSON 
    size_t pos = 0;
    std::string cleanJson = json;
    
    cleanJson.erase(std::remove(cleanJson.begin(), cleanJson.end(), ' '), cleanJson.end());
    cleanJson.erase(std::remove(cleanJson.begin(), cleanJson.end(), '\n'), cleanJson.end());
    
    if (cleanJson.front() == '{') cleanJson.erase(0, 1);
    if (cleanJson.back() == '}') cleanJson.pop_back();
    
    size_t start = 0;
    while ((pos = cleanJson.find(',', start)) != std::string::npos) {
        std::string pair = cleanJson.substr(start, pos - start);
        size_t colonPos = pair.find(':');
        
        if (colonPos != std::string::npos) {
            std::string key = pair.substr(0, colonPos);
            std::string value = pair.substr(colonPos + 1);
            
            if (key.front() == '"') key.erase(0, 1);
            if (key.back() == '"') key.pop_back();
            if (value.front() == '"') value.erase(0, 1);
            if (value.back() == '"') value.pop_back();
            
            result[key] = value;
        }
        
        start = pos + 1;
    }
    
    // Последняя пара
    if (start < cleanJson.length()) {
        std::string pair = cleanJson.substr(start);
        size_t colonPos = pair.find(':');
        
        if (colonPos != std::string::npos) {
            std::string key = pair.substr(0, colonPos);
            std::string value = pair.substr(colonPos + 1);
            
            if (key.front() == '"') key.erase(0, 1);
            if (key.back() == '"') key.pop_back();
            if (value.front() == '"') value.erase(0, 1);
            if (value.back() == '"') value.pop_back();
            
            result[key] = value;
        }
    }
    
    return result;
}

std::string DataAdapter::mapToJson(const std::map<std::string, std::string>& data) {
    std::stringstream json;
    json << "{";
    
    bool first = true;
    for (const auto& pair : data) {
        if (!first) {
            json << ",";
        }
        json << "\"" << pair.first << "\":\"" << pair.second << "\"";
        first = false;
    }
    
    json << "}";
    return json.str();
}

std::string DataAdapter::adaptForFrontend(const std::string& backendData) {
    // Добавляем структуру, ожидаемую фронтендом
    std::string adapted = "{\"data\":" + backendData + "}";
    return adapted;
}