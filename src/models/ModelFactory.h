// src/models/ModelFactory.h
#ifndef MODEL_FACTORY_H
#define MODEL_FACTORY_H

#include "Workout.h"
#include <memory>

class ModelFactory {
public:
    static ModelFactory& getInstance();
    
    Workout createRunning(int minutes);
    Workout createCycling(int minutes);
    Workout createStrength(int minutes);
    
private:
    ModelFactory() = default;
    ~ModelFactory() = default;
    ModelFactory(const ModelFactory&) = delete;
    ModelFactory& operator=(const ModelFactory&) = delete;
};

#endif // MODEL_FACTORY_H