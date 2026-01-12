#ifndef DATA_ADAPTER_H
#define DATA_ADAPTER_H

#include <string>
#include <map>

class DataAdapter {
public:
    static std::map<std::string, std::string> jsonToMap(const std::string& json);
    static std::string mapToJson(const std::map<std::string, std::string>& data);
    static std::string adaptForFrontend(const std::string& backendData);
};

#endif // DATA_ADAPTER_H