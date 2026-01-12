#include "AuthService.h"
#include <ctime>
#include <sstream>
#include <random>

std::map<std::string, int> AuthService::activeTokens;

std::string AuthService::generateToken(int userId) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    
    std::stringstream token;
    token << "ps_token_" << userId << "_" << time(0) << "_" << dis(gen);
    
    // Хэшируем токен 
    std::string tokenStr = token.str();
    activeTokens[tokenStr] = userId;
    
    return tokenStr;
}

bool AuthService::validateToken(const std::string& token) {
    return activeTokens.find(token) != activeTokens.end();
}

int AuthService::getUserIdFromToken(const std::string& token) {
    auto it = activeTokens.find(token);
    if (it != activeTokens.end()) {
        return it->second;
    }
    return 0;
}

void AuthService::invalidateToken(int userId) {
    for (auto it = activeTokens.begin(); it != activeTokens.end();) {
        if (it->second == userId) {
            it = activeTokens.erase(it);
        } else {
            ++it;
        }
    }
}