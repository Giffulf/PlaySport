#include "ModelFactory.h"
#include "User.h"
#include "Workout.h"
#include "Goal.h"

std::unique_ptr<ModelBase> ModelFactory::createModel(ModelType type) {
    switch (type) {
        case USER_MODEL:
            return std::make_unique<User>();
        case WORKOUT_MODEL:
            return std::make_unique<Workout>();
        case GOAL_MODEL:
            return std::make_unique<Goal>();
        default:
            return nullptr;
    }
}

std::unique_ptr<User> ModelFactory::createUser() {
    return std::make_unique<User>();
}

std::unique_ptr<Workout> ModelFactory::createWorkout() {
    return std::make_unique<Workout>();
}

std::unique_ptr<Goal> ModelFactory::createGoal() {
    return std::make_unique<Goal>();
}