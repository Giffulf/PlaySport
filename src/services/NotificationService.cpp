#include "NotificationService.h"
#include <iostream>

NotificationService& NotificationService::getInstance() {
    static NotificationService instance;
    return instance;
}

void NotificationService::subscribe(Observer observer) {
    std::lock_guard<std::mutex> lock(mutex);
    observers.push_back(observer);
}

void NotificationService::notify(const std::string& event, const std::string& data) {
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& observer : observers) {
        observer(event, data);
    }
}