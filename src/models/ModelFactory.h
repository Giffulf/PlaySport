#ifndef MODEL_FACTORY_H
#define MODEL_FACTORY_H

#include <memory>

// Простые фабричные методы без сложной иерархии
class ModelFactory {
public:
    template<typename T>
    static std::unique_ptr<T> create() {
        return std::make_unique<T>();
    }
};

#endif // MODEL_FACTORY_H