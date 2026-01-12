#ifndef GOAL_H
#define GOAL_H

#include "ModelFactory.h"
#include <string>
#include <vector>

class Goal : public ModelBase {
private:
    int id;
    std::string text;
    bool completed;
    int userId;
    
public:
    Goal();
    
    static std::vector<Goal> getUserGoals(int userId);
    bool save();
    bool update();
    bool remove();
    
    int getId() const { return id; }
    std::string getText() const { return text; }
    bool isCompleted() const { return completed; }
    int getUserId() const { return userId; }
    
    void setId(int i) { id = i; }  
    void setText(const std::string& t) { text = t; }
    void setCompleted(bool c) { completed = c; }
    void setUserId(int uid) { userId = uid; }
    
    void toggle();
    
    std::string toJson() const override;
    void fromJson(const std::string& json) override;
};

#endif // GOAL_H