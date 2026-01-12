#ifndef WORKOUT_H
#define WORKOUT_H

#include <string>
#include <vector>

class Workout {
private:
    int id;
    std::string name;
    std::string description;
    int duration;
    int calories;
    std::string type;
    int userId;
    
public:
    Workout();
    
    // Геттеры
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    int getDuration() const { return duration; }
    int getCalories() const { return calories; }
    std::string getType() const { return type; }
    int getUserId() const { return userId; }
    
    // Сеттеры
    void setId(int i) { id = i; }
    void setName(const std::string& n) { name = n; }
    void setDescription(const std::string& d) { description = d; }
    void setDuration(int d) { duration = d; }
    void setCalories(int c) { calories = c; }
    void setType(const std::string& t) { type = t; }
    void setUserId(int uid) { userId = uid; }
    
    // Методы
    bool save();
    bool update();
    bool remove();
    std::string toJson() const;
    void fromJson(const std::string& json);
    
    // Статические методы
    static std::vector<Workout> getUserWorkouts(int userId);
};

#endif // WORKOUT_H