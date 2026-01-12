#ifndef JSON_VIEW_H
#define JSON_VIEW_H

#include <string>
#include <map>

class JsonView {
public:
    static std::string render(const std::map<std::string, std::string>& data);
    static std::string error(const std::string& message, int code = 400);
    static std::string success(const std::string& message, const std::map<std::string, std::string>& data = {});
};

#endif // JSON_VIEW_H