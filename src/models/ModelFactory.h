#ifndef MODEL_FACTORY_H
#define MODEL_FACTORY_H

#include <memory>
#include "User.h"
#include "Workout.h"
#include "Goal.h"

// Интерфейс для моделей
class ModelBase {
public:
    virtual ~ModelBase() = default;
    virtual std::string toJson() const = 0;
    virtual void fromJson(const std::string& json) = 0;
};

// Фабрика моделей (Factory Pattern)
class ModelFactory {
public:
    enum ModelType {
        USER_MODEL,
        WORKOUT_MODEL,
        GOAL_MODEL
    };
    
    static std::unique_ptr<ModelBase> createModel(ModelType type);
    
    // Простые фабричные методы
    static User* createUser();
    static Workout* createWorkout();
    static Goal* createGoal();
};

#endif // MODEL_FACTORY_H