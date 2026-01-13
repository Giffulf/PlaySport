#include "MainController.h"
#include "../models/User.h"
#include "../models/Workout.h"
#include "../models/Goal.h"
#include "../adapters/DataAdapter.h"
#include "../views/JsonView.h"
#include <sstream>
#include <map>
#include <memory>

std::string MainController::handleRequest(
    const std::string& method, 
    const std::string& path, 
    const std::string& body) {
    
    // API Endpoints
    if (path == "/api/login" && method == "POST") {
        return handleLogin(body);
    }
    else if (path == "/api/register" && method == "POST") {
        return handleRegister(body);
    }
    else if (path == "/api/users" && method == "GET") {
        return handleGetUsers();
    }
    else if (path == "/api/workouts" && method == "GET") {
        return handleGetWorkouts();
    }
    else if (path == "/api/goals" && method == "GET") {
        return handleGetGoals();
    }
    else if (path == "/api/stats" && method == "GET") {
        return handleGetStats();
    }
    
    return JsonView::error("Not Found", 404);
}

std::string MainController::handleLogin(const std::string& body) {
    auto data = DataAdapter::jsonToMap(body);
    std::string email = data["email"];
    std::string password = data["password"];
    
    if (email.empty() || password.empty()) {
        return JsonView::error("Email and password required");
    }
    
    // Проверка демо-учетных данных
    if (email == "test@example.com" && password == "password") {
        std::map<std::string, std::string> userData;
        userData["id"] = "1";
        userData["name"] = "Test User";
        userData["email"] = email;
        userData["activeWorkouts"] = "5";
        userData["totalCalories"] = "2450";
        
        std::map<std::string, std::string> response;
        response["token"] = "demo_token_123";
        response["user"] = JsonView::render(userData);
        
        return JsonView::success("Login successful", response);
    }
    
    return JsonView::error("Invalid credentials");
}

std::string MainController::handleRegister(const std::string& body) {
    auto data = DataAdapter::jsonToMap(body);
    
    if (data["email"].empty() || data["password"].empty() || data["name"].empty()) {
        return JsonView::error("All fields are required");
    }
    
    std::map<std::string, std::string> response;
    response["userId"] = "100";
    response["email"] = data["email"];
    
    return JsonView::success("Registration successful", response);
}

std::string MainController::handleGetUsers() {
    std::stringstream json;
    json << "[";
    json << "{\"id\":1,\"name\":\"Иван Петров\",\"email\":\"ivan@example.com\",\"activeWorkouts\":5},";
    json << "{\"id\":2,\"name\":\"Мария Иванова\",\"email\":\"maria@example.com\",\"activeWorkouts\":3},";
    json << "{\"id\":3,\"name\":\"Алексей Смирнов\",\"email\":\"alex@example.com\",\"activeWorkouts\":7}";
    json << "]";
    
    std::map<std::string, std::string> response;
    response["users"] = json.str();
    
    return JsonView::success("Users retrieved", response);
}

std::string MainController::handleGetWorkouts() {
    // Использование модели (Model) - создаем объект модели
    Workout workoutModel;
    
    std::stringstream json;
    json << "[";
    json << "{\"id\":1,\"name\":\"Утренняя пробежка\",\"duration\":30,\"calories\":300,\"type\":\"running\"},";
    json << "{\"id\":2,\"name\":\"Силовая тренировка\",\"duration\":45,\"calories\":350,\"type\":\"strength\"},";
    json << "{\"id\":3,\"name\":\"Велосипед\",\"duration\":60,\"calories\":480,\"type\":\"cycling\"}";
    json << "]";
    
    std::map<std::string, std::string> response;
    response["workouts"] = json.str();
    
    return JsonView::success("Workouts retrieved", response);
}

std::string MainController::handleGetGoals() {
    // Использование модели (Model) - создаем объект модели
    Goal goalModel;
    
    std::stringstream json;
    json << "[";
    json << "{\"id\":1,\"text\":\"Пробежать 5 км без остановки\",\"completed\":false},";
    json << "{\"id\":2,\"text\":\"30 отжиманий за подход\",\"completed\":true},";
    json << "{\"id\":3,\"text\":\"Сбросить 5 кг к декабрю\",\"completed\":false}";
    json << "]";
    
    std::map<std::string, std::string> response;
    response["goals"] = json.str();
    
    return JsonView::success("Goals retrieved", response);
}

std::string MainController::handleGetStats() {
    std::map<std::string, std::string> stats;
    stats["activeWorkouts"] = "12";
    stats["caloriesBurned"] = "3842";
    stats["monthProgress"] = "75";
    stats["totalDistance"] = "42.5";
    
    return JsonView::success("Stats retrieved", stats);
}