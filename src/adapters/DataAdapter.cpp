#include "DataAdapter.h"
#include <sstream>
#include <algorithm>

std::map<std::string, std::string> DataAdapter::jsonToMap(const std::string& json) {
    std::map<std::string, std::string> result;
    std::string cleanJson = json;
    
    // Удаляем пробелы и переносы строк
    cleanJson.erase(std::remove(cleanJson.begin(), cleanJson.end(), ' '), cleanJson.end());
    cleanJson.erase(std::remove(cleanJson.begin(), cleanJson.end(), '\n'), cleanJson.end());
    cleanJson.erase(std::remove(cleanJson.begin(), cleanJson.end(), '\r'), cleanJson.end());
    
    // Удаляем фигурные скобки
    if (cleanJson.front() == '{') cleanJson.erase(0, 1);
    if (cleanJson.back() == '}') cleanJson.pop_back();
    
    size_t start = 0;
    while (true) {
        size_t commaPos = cleanJson.find(',', start);
        std::string pair;
        
        if (commaPos == std::string::npos) {
            pair = cleanJson.substr(start);
        } else {
            pair = cleanJson.substr(start, commaPos - start);
        }
        
        size_t colonPos = pair.find(':');
        if (colonPos != std::string::npos) {
            std::string key = pair.substr(0, colonPos);
            std::string value = pair.substr(colonPos + 1);
            
            // Удаляем кавычки
            if (key.front() == '"' && key.back() == '"') {
                key = key.substr(1, key.length() - 2);
            }
            if (value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.length() - 2);
            }
            
            result[key] = value;
        }
        
        if (commaPos == std::string::npos) break;
        start = commaPos + 1;
    }
    
    return result;
}

std::string DataAdapter::mapToJson(const std::map<std::string, std::string>& data) {
    std::stringstream json;
    json << "{";
    
    bool first = true;
    for (const auto& pair : data) {
        if (!first) json << ",";
        json << "\"" << pair.first << "\":\"" << pair.second << "\"";
        first = false;
    }
    
    json << "}";
    return json.str();
}

std::string DataAdapter::adaptForFrontend(const std::string& backendData) {
    std::stringstream adapted;
    adapted << "{";
    adapted << "\"success\":true,";
    adapted << "\"data\":" << backendData;
    adapted << "}";
    return adapted.str();
}