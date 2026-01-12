#ifndef AUTH_STRATEGY_H
#define AUTH_STRATEGY_H

#include <string>

class AuthStrategy {
public:
    virtual ~AuthStrategy() = default;
    virtual bool authenticate(const std::string& credentials) = 0;
    virtual std::string generateToken() = 0;
};

class JwtAuthStrategy : public AuthStrategy {
public:
    bool authenticate(const std::string& credentials) override;
    std::string generateToken() override;
};

class BasicAuthStrategy : public AuthStrategy {
public:
    bool authenticate(const std::string& credentials) override;
    std::string generateToken() override;
};

#endif // AUTH_STRATEGY_H