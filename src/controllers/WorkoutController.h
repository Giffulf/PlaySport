#ifndef WORKOUT_CONTROLLER_H
#define WORKOUT_CONTROLLER_H

#include <string>
#include <vector>

class WorkoutController {
public:
    static std::string getUserWorkouts(int userId);
    static std::string createWorkout(int userId, const std::string& workoutData);
    static std::string updateWorkout(int workoutId, const std::string& workoutData);
    static std::string deleteWorkout(int workoutId);
    static std::string startWorkout(int workoutId);
};

#endif // WORKOUT_CONTROLLER_H