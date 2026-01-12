#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include <string>
#include <map>

class AuthService {
private:
    static std::map<std::string, int> activeTokens; // token -> userId
    
public:
    static std::string generateToken(int userId);
    static bool validateToken(const std::string& token);
    static int getUserIdFromToken(const std::string& token);
    static void invalidateToken(int userId);
};

#endif // AUTH_SERVICE_H