#include "ModelFactory.h"

// Простая реализация фабрики
std::unique_ptr<ModelBase> ModelFactory::createModel(ModelType type) {
    return nullptr; // В этой версии не используем сложную фабрику
}

// Простые фабричные методы
User* ModelFactory::createUser() {
    return new User();
}

Workout* ModelFactory::createWorkout() {
    return new Workout();
}

Goal* ModelFactory::createGoal() {
    return new Goal();
}