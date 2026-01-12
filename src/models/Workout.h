// src/models/Workout.h
#ifndef WORKOUT_H
#define WORKOUT_H

#include <string>
#include <sstream>

class Workout {
private:
    int id;
    std::string name;
    int duration;
    int calories;
    std::string type;
    
public:
    Workout();
    
    void setId(int i);
    void setName(const std::string& n);
    void setDuration(int d);
    void setCalories(int c);
    void setType(const std::string& t);
    
    int getId() const { return id; }
    std::string getName() const { return name; }
    int getDuration() const { return duration; }
    int getCalories() const { return calories; }
    std::string getType() const { return type; }
    
    std::string toJson() const;
};

#endif // WORKOUT_H