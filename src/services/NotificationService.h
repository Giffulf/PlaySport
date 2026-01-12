#ifndef NOTIFICATION_SERVICE_H
#define NOTIFICATION_SERVICE_H

#include <string>
#include <vector>
#include <functional>
#include <mutex>

class NotificationService {
public:
    using Observer = std::function<void(const std::string&, const std::string&)>;
    
    static NotificationService& getInstance();
    
    void subscribe(Observer observer);
    void notify(const std::string& event, const std::string& data);
    
private:
    NotificationService() = default;
    std::vector<Observer> observers;
    std::mutex mutex;
};

#endif // NOTIFICATION_SERVICE_H