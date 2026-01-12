cmake_minimum_required(VERSION 3.10)
project(playsport)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем библиотеки для Windows
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lws2_32")
endif()

# Основной исполняемый файл
add_executable(playsport src/main.cpp)

# Подключаем библиотеки для Windows
if(WIN32)
    target_link_libraries(playsport ws2_32)
endif()

# Копируем статические файлы
add_custom_command(TARGET playsport POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/public
    ${CMAKE_BINARY_DIR}/public
)