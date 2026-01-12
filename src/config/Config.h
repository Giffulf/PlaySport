#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <mutex>

class Config {
private:
    static Config* instance;
    static std::mutex mutex;
    
    std::map<std::string, std::string> settings;
    
    Config(); // Приватный конструктор
    
public:
    // Удаляем копирование
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // Получение экземпляра (Singleton)
    static Config* getInstance();
    
    // Загрузка конфигурации
    void load(const std::string& filename = "config.json");
    
    // Получение значения
    std::string get(const std::string& key, const std::string& defaultValue = "");
    
    // Установка значения
    void set(const std::string& key, const std::string& value);
    
    // Получение порта сервера
    int getPort() const;
    
    // Получение строки подключения к БД
    std::string getDatabasePath() const;
};

#endif // CONFIG_H