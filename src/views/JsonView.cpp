#include "JsonView.h"
#include <sstream>

std::string JsonView::render(const std::map<std::string, std::string>& data) {
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

std::string JsonView::error(const std::string& message, int code) {
    std::stringstream json;
    json << "{";
    json << "\"success\":false,";
    json << "\"error\":\"" << message << "\",";
    json << "\"code\":" << code;
    json << "}";
    return json.str();
}

std::string JsonView::success(const std::string& message, const std::map<std::string, std::string>& data) {
    std::stringstream json;
    json << "{";
    json << "\"success\":true,";
    json << "\"message\":\"" << message << "\"";
    
    if (!data.empty()) {
        json << ",";
        bool first = true;
        for (const auto& pair : data) {
            if (!first) json << ",";
            json << "\"" << pair.first << "\":\"" << pair.second << "\"";
            first = false;
        }
    }
    
    json << "}";
    return json.str();
}