#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <vector>
#include <atomic>
#include <functional>
#include <memory>
#include <algorithm>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

// =========== КОНФИГУРАЦИЯ ===========
class Config {
private:
    static Config* instance;
    static std::mutex mutex;
    std::map<std::string, std::string> settings;
    
    Config() {
        settings["port"] = "8080";
        settings["static_dir"] = "public";
    }
    
public:
    static Config* getInstance() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!instance) instance = new Config();
        return instance;
    }
    
    std::string get(const std::string& key, const std::string& defaultValue = "") {
        auto it = settings.find(key);
        return it != settings.end() ? it->second : defaultValue;
    }
    
    int getPort() { return std::stoi(get("port", "8080")); }
};

Config* Config::instance = nullptr;
std::mutex Config::mutex;

// =========== ОЧЕРЕДЬ ЗАДАЧ ===========
class TaskQueue {
private:
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::vector<std::thread> workers;
    bool stop = false;

public:
    TaskQueue(size_t numThreads = 2) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { 
                            return stop || !tasks.empty(); 
                        });
                        
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
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        
        for (auto& worker : workers) {
            if (worker.joinable()) worker.join();
        }
    }

    void enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.push(task);
        }
        condition.notify_one();
    }
};

// =========== JSON УТИЛИТЫ ===========
class JsonUtils {
public:
    static std::map<std::string, std::string> parseJson(const std::string& json) {
        std::map<std::string, std::string> result;
        std::string clean = json;
        
        // Упрощенный парсинг JSON
        clean.erase(std::remove(clean.begin(), clean.end(), ' '), clean.end());
        clean.erase(std::remove(clean.begin(), clean.end(), '\n'), clean.end());
        clean.erase(std::remove(clean.begin(), clean.end(), '\r'), clean.end());
        
        if (clean.front() == '{') clean.erase(0, 1);
        if (clean.back() == '}') clean.pop_back();
        
        size_t start = 0;
        while (true) {
            size_t comma = clean.find(',', start);
            std::string pair;
            
            if (comma == std::string::npos) {
                pair = clean.substr(start);
            } else {
                pair = clean.substr(start, comma - start);
            }
            
            size_t colon = pair.find(':');
            if (colon != std::string::npos) {
                std::string key = pair.substr(0, colon);
                std::string value = pair.substr(colon + 1);
                
                if (key.front() == '"' && key.back() == '"') 
                    key = key.substr(1, key.length() - 2);
                if (value.front() == '"' && value.back() == '"') 
                    value = value.substr(1, value.length() - 2);
                
                result[key] = value;
            }
            
            if (comma == std::string::npos) break;
            start = comma + 1;
        }
        
        return result;
    }
    
    static std::string createError(const std::string& message) {
        return "{\"success\":false,\"error\":\"" + message + "\"}";
    }
    
    static std::string createSuccess(const std::string& data = "") {
        if (data.empty()) return "{\"success\":true}";
        return "{\"success\":true,\"data\":" + data + "}";
    }
};

// =========== HTTP СЕРВЕР ===========
class HttpServer {
private:
    int port;
    std::string publicDir;
#ifdef _WIN32
    SOCKET serverSocket;
#else
    int serverSocket;
#endif
    std::atomic<bool> running{false};
    std::thread serverThread;
    TaskQueue taskQueue;

public:
    HttpServer(int port = 8080, const std::string& publicDir = "public")
        : port(port), publicDir(publicDir)
#ifdef _WIN32
        , serverSocket(INVALID_SOCKET)
#else
        , serverSocket(-1)
#endif
        , taskQueue(4) {
    }

    ~HttpServer() { stop(); }

    void start() {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed" << std::endl;
            return;
        }
#endif

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed" << std::endl;
            WSACleanup();
            return;
        }
#else
        if (serverSocket < 0) {
            std::cerr << "Socket creation failed" << std::endl;
            return;
        }
#endif

        int opt = 1;
#ifdef _WIN32
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
#else
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Bind failed" << std::endl;
            closeSocket();
            return;
        }

        if (listen(serverSocket, 10) < 0) {
            std::cerr << "Listen failed" << std::endl;
            closeSocket();
            return;
        }

        running = true;
        serverThread = std::thread(&HttpServer::run, this);

        std::cout << "HTTP Server started on port " << port << std::endl;
        std::cout << "Serving static files from: " << publicDir << std::endl;
    }

    void stop() {
        running = false;
        closeSocket();
        if (serverThread.joinable()) {
            serverThread.join();
        }
    }

private:
    void closeSocket() {
#ifdef _WIN32
        if (serverSocket != INVALID_SOCKET) {
            closesocket(serverSocket);
            serverSocket = INVALID_SOCKET;
        }
        WSACleanup();
#else
        if (serverSocket >= 0) {
            close(serverSocket);
            serverSocket = -1;
        }
#endif
    }

    std::string readFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) return "";
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        return content;
    }

    std::string getContentType(const std::string& path) {
        if (path.find(".html") != std::string::npos) return "text/html; charset=utf-8";
        if (path.find(".css") != std::string::npos) return "text/css";
        if (path.find(".js") != std::string::npos) return "application/javascript";
        if (path.find(".json") != std::string::npos) return "application/json";
        return "text/plain";
    }

    std::string createResponse(int status, const std::string& contentType, const std::string& body) {
        std::stringstream response;
        response << "HTTP/1.1 " << status << " " 
                 << (status == 200 ? "OK" : "Not Found") << "\r\n";
        response << "Content-Type: " << contentType << "\r\n";
        response << "Content-Length: " << body.length() << "\r\n";
        response << "Access-Control-Allow-Origin: *\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
        response << body;
        return response.str();
    }

    std::string handleApiRequest(const std::string& method, const std::string& path, const std::string& body) {
        // Login
        if (path == "/api/login" && method == "POST") {
            auto data = JsonUtils::parseJson(body);
            std::string email = data["email"];
            std::string password = data["password"];
            
            if (email.empty() || password.empty()) {
                return JsonUtils::createError("Email and password required");
            }
            
            if (email == "test@example.com" && password == "password") {
                std::string userJson = R"({"id":1,"name":"Test User","email":"test@example.com"})";
                return R"({"success":true,"token":"demo_token","user":)" + userJson + "}";
            }
            
            return JsonUtils::createError("Invalid credentials");
        }
        
        // Register
        else if (path == "/api/register" && method == "POST") {
            auto data = JsonUtils::parseJson(body);
            
            if (data["email"].empty() || data["password"].empty() || data["name"].empty()) {
                return JsonUtils::createError("All fields are required");
            }
            
            return R"({"success":true,"message":"Registration successful","userId":100})";
        }
        
        // Workouts
        else if (path == "/api/workouts" && method == "GET") {
            return JsonUtils::createSuccess(R"([
                {"id":1,"name":"Утренняя пробежка","duration":30,"calories":300,"type":"running"},
                {"id":2,"name":"Силовая тренировка","duration":45,"calories":350,"type":"strength"},
                {"id":3,"name":"Велосипед","duration":60,"calories":480,"type":"cycling"}
            ])");
        }
        
        // Stats
        else if (path == "/api/stats" && method == "GET") {
            return JsonUtils::createSuccess(R"({
                "activeWorkouts":12,
                "caloriesBurned":3842,
                "monthProgress":75,
                "totalDistance":42.5
            })");
        }
        
        // Goals
        else if (path == "/api/goals" && method == "GET") {
            return JsonUtils::createSuccess(R"([
                {"id":1,"text":"Пробежать 5 км без остановки","completed":false},
                {"id":2,"text":"30 отжиманий за подход","completed":true},
                {"id":3,"text":"Сбросить 5 кг к декабрю","completed":false}
            ])");
        }
        
        else {
            return JsonUtils::createError("Endpoint not found");
        }
    }

    void handleClient(int clientSocket) {
        char buffer[4096];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string request(buffer);
            
            // Парсинг запроса
            std::istringstream stream(request);
            std::string method, path, protocol;
            stream >> method >> path >> protocol;
            
            // Простой парсинг
            std::string line;
            std::string body;
            while (std::getline(stream, line) && line != "\r") {
                // Пропускаем заголовки
            }
            
            // Читаем оставшееся как тело
            std::getline(stream, body, '\0');
            
            std::string response;
            
            // API запросы
            if (path.find("/api/") == 0) {
                response = createResponse(200, "application/json", 
                    handleApiRequest(method, path, body));
            }
            // Статические файлы
            else {
                std::string filePath = publicDir + (path == "/" ? "/index.html" : path);
                std::string content = readFile(filePath);
                
                if (!content.empty()) {
                    response = createResponse(200, getContentType(filePath), content);
                } else {
                    // Демо HTML если файла нет
                    std::string demoHtml = R"(<!DOCTYPE html>
<html>
<head><title>PlaySport</title></head>
<body>
    <h1>PlaySport Backend</h1>
    <p>Server is running!</p>
    <p>Use API endpoints:</p>
    <ul>
        <li>GET /api/workouts</li>
        <li>GET /api/stats</li>
        <li>GET /api/goals</li>
        <li>POST /api/login</li>
    </ul>
</body>
</html>)";
                    response = createResponse(200, "text/html", demoHtml);
                }
            }
            
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        
#ifdef _WIN32
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif
    }

    void run() {
        while (running) {
            sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            
#ifdef _WIN32
            SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
            if (clientSocket == INVALID_SOCKET) {
                if (running) std::cerr << "Accept failed" << std::endl;
                break;
            }
#else
            int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
            if (clientSocket < 0) {
                if (running) std::cerr << "Accept failed" << std::endl;
                break;
            }
#endif
            
            taskQueue.enqueue([this, clientSocket]() {
                handleClient(clientSocket);
            });
        }
    }
};

// =========== MAIN ===========
int main() {
    std::cout << "========================================\n";
    std::cout << "       PLAYSPORT BACKEND SYSTEM\n";
    std::cout << "========================================\n\n";
    
    Config* config = Config::getInstance();
    HttpServer server(config->getPort());
    
    // Создаем папку public
#ifdef _WIN32
    system("mkdir public 2>nul");
#else
    system("mkdir -p public");
#endif
    
    server.start();
    
    std::cout << "\nServer running on http://localhost:" << config->getPort() << "\n";
    std::cout << "Press Enter to stop...\n";
    
    std::cin.get();
    server.stop();
    
    std::cout << "Server stopped.\n";
    return 0;
}