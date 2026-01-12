#ifndef MODEL_BASE_H
#define MODEL_BASE_H

#include <string>

class ModelBase {
public:
    virtual ~ModelBase() = default;
    virtual std::string toJson() const = 0;
    virtual void fromJson(const std::string& json) = 0;
};

#endif // MODEL_BASE_H