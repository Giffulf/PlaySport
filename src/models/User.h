#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <functional>

class User {
private:
    int id;
    std::string name;
    std::string email;
    std::string passwordHash;
    std::string createdAt;
    int activeWorkouts;
    int totalCalories;
    
public:
    User();
    
    static std::vector<User> getAll();
    static User getById(int id);
    static User getByEmail(const std::string& email);
    bool save();
    bool update();
    bool remove();
    
    // Геттеры
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
    std::string getCreatedAt() const { return createdAt; }
    int getActiveWorkouts() const { return activeWorkouts; }
    int getTotalCalories() const { return totalCalories; }
    
    // Сеттеры
    void setId(int i) { id = i; }  
    void setName(const std::string& n) { name = n; }
    void setEmail(const std::string& e) { email = e; }
    void setPassword(const std::string& password);
    void setActiveWorkouts(int count) { activeWorkouts = count; }
    void setTotalCalories(int calories) { totalCalories = calories; }
    
    bool verifyPassword(const std::string& password) const;
    
    std::string toJson() const;
    void fromJson(const std::string& json);
    
private:
    std::string hashPassword(const std::string& password) const;
};

#endif // USER_H