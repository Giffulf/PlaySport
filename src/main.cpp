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
#include <memory>
#include <algorithm>
#include <functional>

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

#include "config/Config.h"
#include "controllers/MainController.h"
#include "services/TaskQueue.h"

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

    void handleClient(int clientSocket) {
        char buffer[4096];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string request(buffer);
            
            std::istringstream stream(request);
            std::string method, path, protocol;
            stream >> method >> path >> protocol;
            
            std::string line;
            std::string body;
            while (std::getline(stream, line) && line != "\r") {}
            
            std::getline(stream, body, '\0');
            
            std::string response;
            
            // API запросы
            if (path.find("/api/") == 0) {
                std::string apiResponse = MainController::handleRequest(method, path, body);
                response = createResponse(200, "application/json", apiResponse);
            }
            // Статические файлы
            else {
                std::string filePath = publicDir + (path == "/" ? "/index.html" : path);
                std::string content = readFile(filePath);
                
                if (!content.empty()) {
                    response = createResponse(200, getContentType(filePath), content);
                } else {
                    std::string demoHtml = R"(<!DOCTYPE html>
<html>
<head><title>PlaySport</title></head>
<body>
    <h1>PlaySport Backend</h1>
    <p>Server is running!</p>
    <p>Use API endpoints:</p>
    <ul>
        <li>GET /api/users</li>
        <li>GET /api/workouts</li>
        <li>GET /api/goals</li>
        <li>GET /api/stats</li>
        <li>POST /api/login</li>
        <li>POST /api/register</li>
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
    
    std::cout << "Architecture Patterns Used:\n";
    std::cout << "1. Singleton Pattern (Config)\n";
    std::cout << "2. Factory Pattern (ModelFactory)\n";
    std::cout << "3. Strategy Pattern (AuthStrategy)\n";
    std::cout << "4. Observer Pattern (NotificationService)\n";
    std::cout << "5. Producer-Consumer Pattern (TaskQueue)\n";
    std::cout << "6. Adapter Pattern (DataAdapter)\n";
    std::cout << "7. MVC Pattern (Controllers, Models, Views)\n\n";
    
    Config* config = Config::getInstance();
    HttpServer server(config->getPort());
    
#ifdef _WIN32
    system("mkdir public 2>nul");
#else
    system("mkdir -p public");
#endif
    
    server.start();
    
    std::cout << "\nServer running on http://localhost:" << config->getPort() << "\n";
    std::cout << "Frontend available at: http://localhost:" << config->getPort() << "/index.html\n";
    std::cout << "Press Enter to stop...\n";
    
    std::cin.get();
    server.stop();
    
    std::cout << "Server stopped.\n";
    return 0;
}