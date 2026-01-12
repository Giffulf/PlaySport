#include "AuthStrategy.h"
#include <ctime>
#include <sstream>

bool JwtAuthStrategy::authenticate(const std::string& credentials) {
    // Проверка JWT токена
    return !credentials.empty() && credentials.find("ps_token_") == 0;
}

std::string JwtAuthStrategy::generateToken() {
    std::stringstream token;
    token << "ps_token_" << time(0);
    return token.str();
}

bool BasicAuthStrategy::authenticate(const std::string& credentials) {
    // Проверка базовой аутентификации (username:password)
    return credentials.find(':') != std::string::npos;
}

std::string BasicAuthStrategy::generateToken() {
    return "basic_auth_token";
}