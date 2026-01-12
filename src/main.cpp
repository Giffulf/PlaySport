#include <iostream>
#include <thread>
#include <chrono>
#include <map>
#include <string>
#include <mutex>
#include <functional>
#include <queue>
#include <condition_variable>
#include <vector>
#include <memory>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <atomic>  
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// ==================== ПАТТЕРНЫ ПРОЕКТИРОВАНИЯ ====================

// === 1. SINGLETON (Одиночка) ===
class Config {
private:
    static Config* instance;
    static std::mutex mutex;
    std::map<std::string, std::string> settings;
    
    Config() {
        settings["port"] = "8080";
        settings["static_dir"] = "public";
        settings["db_path"] = "playsport.db";
    }
    
public:
    static Config* getInstance() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!instance) instance = new Config();
        return instance;
    }
    
    std::string get(const std::string& key) {
        auto it = settings.find(key);
        if (it != settings.end()) return it->second;
        return "";
    }
    
    void set(const std::string& key, const std::string& value) {
        settings[key] = value;
    }
};

Config* Config::instance = nullptr;
std::mutex Config::mutex;

// === 2. FACTORY (Фабрика) ===
class Workout {
private:
    int id;
    std::string name;
    int duration;
    int calories;
    std::string type;
    
public:
    Workout() : id(0), duration(0), calories(0) {}
    
    void setId(int i) { id = i; }
    void setName(const std::string& n) { name = n; }
    void setDuration(int d) { duration = d; }
    void setCalories(int c) { calories = c; }
    void setType(const std::string& t) { type = t; }
    
    std::string toJson() const {
        std::stringstream json;
        json << "{";
        json << "\"id\":" << id << ",";
        json << "\"name\":\"" << name << "\",";
        json << "\"duration\":" << duration << ",";
        json << "\"calories\":" << calories << ",";
        json << "\"type\":\"" << type << "\"";
        json << "}";
        return json.str();
    }
};

class WorkoutFactory {
public:
    static Workout createRunning(int minutes) {
        Workout w;
        w.setName("Running");
        w.setDuration(minutes);
        w.setCalories(minutes * 10);
        w.setType("cardio");
        return w;
    }
    
    static Workout createCycling(int minutes) {
        Workout w;
        w.setName("Cycling");
        w.setDuration(minutes);
        w.setCalories(minutes * 8);
        w.setType("cardio");
        return w;
    }
    
    static Workout createStrength(int minutes) {
        Workout w;
        w.setName("Strength Training");
        w.setDuration(minutes);
        w.setCalories(minutes * 7);
        w.setType("strength");
        return w;
    }
};

// === 3. OBSERVER (Наблюдатель) ===
class NotificationService {
private:
    std::vector<std::function<void(const std::string&, const std::string&)>> observers;
    std::mutex mutex;
    
public:
    static NotificationService& getInstance() {
        static NotificationService instance;
        return instance;
    }
    
    void subscribe(std::function<void(const std::string&, const std::string&)> observer) {
        std::lock_guard<std::mutex> lock(mutex);
        observers.push_back(observer);
    }
    
    void notify(const std::string& event, const std::string& data) {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& observer : observers) {
            observer(event, data);
        }
    }
};

// === 4. STRATEGY (Стратегия) ===
class AuthStrategy {
public:
    virtual ~AuthStrategy() = default;
    virtual bool authenticate(const std::string& credentials) = 0;
    virtual std::string getType() const = 0;
};

class JwtStrategy : public AuthStrategy {
public:
    bool authenticate(const std::string& credentials) override {
        return credentials.find("Bearer ") == 0;
    }
    
    std::string getType() const override {
        return "JWT";
    }
};

class BasicStrategy : public AuthStrategy {
public:
    bool authenticate(const std::string& credentials) override {
        return credentials.find("Basic ") == 0;
    }
    
    std::string getType() const override {
        return "Basic";
    }
};

// === 5. PRODUCER-CONSUMER (Производитель-Потребитель) ===
class TaskQueue {
private:
    std::queue<std::function<void()>> tasks;
    std::mutex mutex;
    std::condition_variable cv;
    std::vector<std::thread> workers;
    bool stop = false;
    
public:
    TaskQueue(size_t numWorkers) {
        for (size_t i = 0; i < numWorkers; ++i) {
            workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mutex);
                        cv.wait(lock, [this]() { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    if (task) task();
                }
            });
        }
    }
    
    ~TaskQueue() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            stop = true;
        }
        cv.notify_all();
        for (auto& worker : workers) {
            worker.join();
        }
    }
    
    void enqueue(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            tasks.push(task);
        }
        cv.notify_one();
    }
};

// === 6. ADAPTER (Адаптер) ===
class DataAdapter {
public:
    static std::string toFrontendFormat(const std::string& backendData, bool success = true) {
        std::stringstream json;
        json << "{";
        json << "\"success\":" << (success ? "true" : "false") << ",";
        json << "\"timestamp\":\"" << getCurrentTime() << "\",";
        json << "\"data\":" << backendData;
        json << "}";
        return json.str();
    }
    
    static std::string createApiResponse(const std::string& data, const std::string& contentType = "application/json") {
        std::stringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: " << contentType << "\r\n";
        response << "Access-Control-Allow-Origin: *\r\n";
        response << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
        response << "Access-Control-Allow-Headers: Content-Type\r\n";
        response << "\r\n";
        response << data;
        return response.str();
    }
    
    static std::string createErrorResponse(const std::string& message, int code = 404) {
        std::stringstream response;
        response << "HTTP/1.1 " << code << " " << (code == 404 ? "Not Found" : "Bad Request") << "\r\n";
        response << "Content-Type: application/json\r\n";
        response << "Access-Control-Allow-Origin: *\r\n";
        response << "\r\n";
        response << "{\"success\":false,\"error\":\"" << message << "\"}";
        return response.str();
    }
    
private:
    static std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        return buffer;
    }
};

// === 7. FACADE (Фасад) ===
class ApiFacade {
private:
    Config* config;
    TaskQueue taskQueue;
    
public:
    ApiFacade() : config(Config::getInstance()), taskQueue(2) {}
    
    std::string handleApiRequest(const std::string& endpoint, const std::string& method, const std::string& body = "") {
        if (endpoint == "/api/workouts" && method == "GET") {
            return getWorkouts();
        }
        else if (endpoint == "/api/stats" && method == "GET") {
            return getStats();
        }
        else if (endpoint == "/api/goals" && method == "GET") {
            return getGoals();
        }
        else if (endpoint == "/api/users" && method == "GET") {
            return getUsers();
        }
        else if (endpoint == "/api/login" && method == "POST") {
            return login(body);
        }
        else if (endpoint == "/api/register" && method == "POST") {
            return registerUser(body);
        }
        else {
            return DataAdapter::toFrontendFormat("\"error\":\"Not found\"", false);
        }
    }
    
private:
    std::string getWorkouts() {
        std::vector<Workout> workouts = {
            WorkoutFactory::createRunning(30),
            WorkoutFactory::createCycling(45),
            WorkoutFactory::createStrength(60)
        };
        
        std::stringstream json;
        json << "[";
        for (size_t i = 0; i < workouts.size(); i++) {
            workouts[i].setId(i + 1);
            json << workouts[i].toJson();
            if (i < workouts.size() - 1) json << ",";
        }
        json << "]";
        
        taskQueue.enqueue([workouts]() {
            std::cout << "[BACKGROUND] Processed " << workouts.size() << " workouts\n";
        });
        
        return DataAdapter::toFrontendFormat(json.str());
    }
    
    std::string getStats() {
        std::stringstream json;
        json << "{";
        json << "\"activeWorkouts\":12,";
        json << "\"caloriesBurned\":3842,";
        json << "\"monthProgress\":75,";
        json << "\"totalDistance\":42.5";
        json << "}";
        return DataAdapter::toFrontendFormat(json.str());
    }
    
    std::string getGoals() {
        std::stringstream json;
        json << "[";
        json << "{\"id\":1,\"text\":\"Пробежать 5 км без остановки\",\"completed\":false},";
        json << "{\"id\":2,\"text\":\"30 отжиманий за подход\",\"completed\":true},";
        json << "{\"id\":3,\"text\":\"Сбросить 5 кг к декабрю\",\"completed\":false}";
        json << "]";
        return DataAdapter::toFrontendFormat(json.str());
    }
    
    std::string getUsers() {
        std::stringstream json;
        json << "[";
        json << "{\"id\":1,\"name\":\"Иван Петров\",\"email\":\"ivan@example.com\",\"activeWorkouts\":5},";
        json << "{\"id\":2,\"name\":\"Мария Иванова\",\"email\":\"maria@example.com\",\"activeWorkouts\":3},";
        json << "{\"id\":3,\"name\":\"Алексей Смирнов\",\"email\":\"alex@example.com\",\"activeWorkouts\":7}";
        json << "]";
        return DataAdapter::toFrontendFormat(json.str());
    }
    
    std::string login(const std::string& body) {
        // Парсим JSON из тела запроса
        std::cout << "Login attempt with body: " << body << std::endl;
        
        std::stringstream json;
        json << "{";
        json << "\"success\":true,";
        json << "\"token\":\"ps_token_123456\",";
        json << "\"user\":{";
        json << "\"id\":1,";
        json << "\"name\":\"Иван Петров\",";
        json << "\"email\":\"ivan@example.com\",";
        json << "\"stats\":{";
        json << "\"activeWorkouts\":5,";
        json << "\"caloriesBurned\":2450,";
        json << "\"monthProgress\":75";
        json << "}";
        json << "}";
        json << "}";
        return json.str();
    }
    
    std::string registerUser(const std::string& body) {
        std::cout << "Registration attempt with body: " << body << std::endl;
        
        std::stringstream json;
        json << "{";
        json << "\"success\":true,";
        json << "\"message\":\"Регистрация успешна!\",";
        json << "\"userId\":100,";
        json << "\"email\":\"newuser@example.com\"";
        json << "}";
        return json.str();
    }
};

// === 8. MVC (Model-View-Controller) ===
class UserModel {
private:
    int id;
    std::string name;
    std::string email;
    
public:
    UserModel(int i, const std::string& n, const std::string& e) 
        : id(i), name(n), email(e) {}
    
    std::string toJson() const {
        std::stringstream json;
        json << "{";
        json << "\"id\":" << id << ",";
        json << "\"name\":\"" << name << "\",";
        json << "\"email\":\"" << email << "\"";
        json << "}";
        return json.str();
    }
};

class JsonView {
public:
    static std::string render(const std::string& data) {
        return DataAdapter::toFrontendFormat(data);
    }
};

class UserController {
private:
    std::vector<UserModel> users;
    
public:
    UserController() {
        users.emplace_back(1, "Иван Петров", "ivan@example.com");
        users.emplace_back(2, "Мария Иванова", "maria@example.com");
        users.emplace_back(3, "Алексей Смирнов", "alex@example.com");
    }
    
    std::string getAllUsers() {
        std::stringstream json;
        json << "[";
        for (size_t i = 0; i < users.size(); i++) {
            json << users[i].toJson();
            if (i < users.size() - 1) json << ",";
        }
        json << "]";
        return JsonView::render(json.str());
    }
};

// ==================== HTTP СЕРВЕР ====================

class HttpServer {
private:
    int port;
    std::string publicDir;
    SOCKET serverSocket;
    std::atomic<bool> running;
    std::thread serverThread;
    ApiFacade apiFacade;
    
    using Handler = std::function<std::string(const std::string&, 
                                              const std::map<std::string, std::string>&,
                                              const std::string&)>;
    std::map<std::string, Handler> handlers;
    
public:
    HttpServer(int port = 8080, const std::string& publicDir = "public") 
        : port(port), publicDir(publicDir), serverSocket(INVALID_SOCKET), running(false) {
        
        // Регистрируем обработчики по умолчанию
        setupHandlers();
    }
    
    ~HttpServer() {
        stop();
    }
    
    void start() {
        // Инициализация Winsock
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            std::cerr << "WSAStartup failed: " << result << std::endl;
            return;
        }
        
        // Создание сокета
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return;
        }
        
        // Настройка сокета
        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
        
        // Привязка сокета
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);
        
        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return;
        }
        
        // Прослушивание
        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return;
        }
        
        running = true;
        serverThread = std::thread(&HttpServer::run, this);
        
        std::cout << "HTTP Server started on port " << port << std::endl;
        std::cout << "Serving static files from: " << publicDir << std::endl;
    }
    
    void stop() {
        running = false;
        if (serverSocket != INVALID_SOCKET) {
            closesocket(serverSocket);
            serverSocket = INVALID_SOCKET;
        }
        if (serverThread.joinable()) {
            serverThread.join();
        }
        WSACleanup();
    }
    
private:
    void setupHandlers() {
        handlers["/api/workouts"] = [this](const std::string& method, 
                                          const std::map<std::string, std::string>& headers,
                                          const std::string& body) {
            if (method == "GET") {
                std::string data = apiFacade.handleApiRequest("/api/workouts", "GET");
                return DataAdapter::createApiResponse(data);
            }
            return DataAdapter::createErrorResponse("Method not allowed");
        };
        
        handlers["/api/stats"] = [this](const std::string& method,
                                       const std::map<std::string, std::string>& headers,
                                       const std::string& body) {
            if (method == "GET") {
                std::string data = apiFacade.handleApiRequest("/api/stats", "GET");
                return DataAdapter::createApiResponse(data);
            }
            return DataAdapter::createErrorResponse("Method not allowed");
        };
        
        handlers["/api/goals"] = [this](const std::string& method,
                                       const std::map<std::string, std::string>& headers,
                                       const std::string& body) {
            if (method == "GET") {
                std::string data = apiFacade.handleApiRequest("/api/goals", "GET");
                return DataAdapter::createApiResponse(data);
            }
            return DataAdapter::createErrorResponse("Method not allowed");
        };
        
        handlers["/api/users"] = [this](const std::string& method,
                                       const std::map<std::string, std::string>& headers,
                                       const std::string& body) {
            if (method == "GET") {
                std::string data = apiFacade.handleApiRequest("/api/users", "GET");
                return DataAdapter::createApiResponse(data);
            }
            return DataAdapter::createErrorResponse("Method not allowed");
        };
        
        handlers["/api/login"] = [this](const std::string& method,
                                       const std::map<std::string, std::string>& headers,
                                       const std::string& body) {
            if (method == "POST") {
                std::string data = apiFacade.handleApiRequest("/api/login", "POST", body);
                return DataAdapter::createApiResponse("{\"success\":true,\"data\":" + data + "}");
            }
            return DataAdapter::createErrorResponse("Method not allowed");
        };
        
        handlers["/api/register"] = [this](const std::string& method,
                                          const std::map<std::string, std::string>& headers,
                                          const std::string& body) {
            if (method == "POST") {
                std::string data = apiFacade.handleApiRequest("/api/register", "POST", body);
                return DataAdapter::createApiResponse("{\"success\":true,\"data\":" + data + "}");
            }
            return DataAdapter::createErrorResponse("Method not allowed");
        };
    }
    
    std::string parseRequest(const std::string& request, 
                            std::string& method, 
                            std::string& path,
                            std::map<std::string, std::string>& headers,
                            std::string& body) {
        std::istringstream stream(request);
        std::string line;
        
        // Первая строка: метод и путь
        if (std::getline(stream, line)) {
            std::istringstream lineStream(line);
            lineStream >> method >> path;
        }
        
        // Заголовки
        while (std::getline(stream, line) && line != "\r" && !line.empty()) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 2);
                // Убираем \r в конце
                if (!value.empty() && value.back() == '\r') {
                    value.pop_back();
                }
                headers[key] = value;
            }
        }
        
        // Тело
        body.clear();
        std::string contentLengthStr = headers["Content-Length"];
        if (!contentLengthStr.empty()) {
            int contentLength = std::stoi(contentLengthStr);
            char* buffer = new char[contentLength + 1];
            stream.read(buffer, contentLength);
            buffer[contentLength] = '\0';
            body = buffer;
            delete[] buffer;
        } else {
            // Читаем остаток
            while (std::getline(stream, line)) {
                body += line + "\n";
            }
        }
        
        return "OK";
    }
    
    std::string serveStaticFile(const std::string& path) {
        std::string filePath = publicDir + (path == "/" ? "/index.html" : path);
        
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            // Если файл не найден, пробуем index.html
            filePath = publicDir + "/index.html";
            file.open(filePath, std::ios::binary);
            
            if (!file) {
                return DataAdapter::createErrorResponse("File not found");
            }
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        std::string contentType = "text/plain";
        std::string ext = filePath.substr(filePath.find_last_of('.') + 1);
        
        if (ext == "html") contentType = "text/html; charset=utf-8";
        else if (ext == "css") contentType = "text/css";
        else if (ext == "js") contentType = "application/javascript";
        else if (ext == "json") contentType = "application/json";
        
        return DataAdapter::createApiResponse(content, contentType);
    }
    
    void handleClient(SOCKET clientSocket) {
        char buffer[4096];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::string request(buffer);
            
            std::string method, path, body;
            std::map<std::string, std::string> headers;
            
            parseRequest(request, method, path, headers, body);
            
            std::cout << "Request: " << method << " " << path << std::endl;
            
            std::string response;
            
            // Проверяем API эндпоинты
            if (handlers.find(path) != handlers.end()) {
                response = handlers[path](method, headers, body);
            } 
            // Статические файлы
            else if (path == "/" || path.find(".") != std::string::npos) {
                response = serveStaticFile(path);
            }
            // API endpoint не найден
            else {
                response = DataAdapter::createErrorResponse("Not Found");
            }
            
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        
        closesocket(clientSocket);
    }
    
    void run() {
        while (running) {
            sockaddr_in clientAddr;
            int clientAddrSize = sizeof(clientAddr);
            
            SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
            if (clientSocket == INVALID_SOCKET) {
                if (running) {
                    std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
                }
                break;
            }
            
            // Обработка клиента в отдельном потоке
            std::thread clientThread(&HttpServer::handleClient, this, clientSocket);
            clientThread.detach();
        }
    }
};

// ==================== ГЛАВНАЯ ПРОГРАММА ====================

int main() {
    std::cout << "========================================\n";
    std::cout << "       PLAYSPORT BACKEND SYSTEM\n";
    std::cout << "========================================\n\n";
    
    // Демонстрация паттернов
    std::cout << "Демонстрация паттернов проектирования:\n\n";
    
    // 1. Singleton
    std::cout << "1. SINGLETON Pattern:\n";
    Config* config = Config::getInstance();
    std::cout << "   Port: " << config->get("port") << "\n";
    
    // 2. Factory
    std::cout << "2. FACTORY Pattern:\n";
    Workout running = WorkoutFactory::createRunning(30);
    std::cout << "   Created workout: " << running.toJson() << "\n";
    
    // 3. Observer
    std::cout << "3. OBSERVER Pattern:\n";
    NotificationService& notifier = NotificationService::getInstance();
    notifier.subscribe([](const std::string& event, const std::string& data) {
        std::cout << "   Event: " << event << ", Data: " << data << "\n";
    });
    notifier.notify("system_start", "PlaySport backend started");
    
    // 4. Strategy
    std::cout << "4. STRATEGY Pattern:\n";
    JwtStrategy jwtStrategy;
    std::cout << "   JWT Auth valid: " << (jwtStrategy.authenticate("Bearer token123") ? "YES" : "NO") << "\n";
    
    // 5. Producer-Consumer
    std::cout << "5. PRODUCER-CONSUMER Pattern:\n";
    TaskQueue queue(2);
    queue.enqueue([]() {
        std::cout << "   [TASK] Processing workout data...\n";
    });
    
    // 6. Adapter
    std::cout << "6. ADAPTER Pattern:\n";
    std::string adapted = DataAdapter::toFrontendFormat("{\"test\":\"data\"}");
    std::cout << "   Adapted data: " << adapted.substr(0, 50) << "...\n";
    
    // 7. Facade
    std::cout << "7. FACADE Pattern:\n";
    ApiFacade apiFacade;
    std::cout << "   API Facade created\n";
    
    // 8. MVC
    std::cout << "8. MVC Pattern:\n";
    UserController userController;
    std::cout << "   UserController created\n\n";
    
    // Создаем HTTP сервер
    std::cout << "========================================\n";
    std::cout << "Запуск HTTP сервера...\n";
    
    HttpServer server(8080, "public");
    
    // Запускаем сервер
    server.start();
    
    std::cout << "Сервер запущен!\n";
    std::cout << "Откройте в браузере: http://localhost:8080\n\n";
    std::cout << "Доступные API endpoints:\n";
    std::cout << "  GET  /api/workouts  - Получить тренировки\n";
    std::cout << "  GET  /api/stats     - Получить статистику\n";
    std::cout << "  GET  /api/goals     - Получить цели\n";
    std::cout << "  GET  /api/users     - Получить пользователей\n";
    std::cout << "  POST /api/login     - Вход в систему\n";
    std::cout << "  POST /api/register  - Регистрация\n";
    std::cout << "========================================\n\n";
    
    std::cout << "Нажмите Enter для остановки сервера...\n";
    std::cin.get();
    
    server.stop();
    std::cout << "Сервер остановлен.\n";
    
    return 0;
}