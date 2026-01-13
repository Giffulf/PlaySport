cmake_minimum_required(VERSION 3.10)
project(playsport)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Для Windows добавляем библиотеку Winsock
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)
    set(WINSOCK_LIB ws2_32)
endif()

# Основной исполняемый файл
add_executable(playsport 
    src/main.cpp
    src/adapters/DataAdapter.cpp
    src/config/Config.cpp
    src/controllers/AuthController.cpp
    src/controllers/UserController.cpp
    src/controllers/WorkoutController.cpp
    src/controllers/MainController.cpp
    src/models/User.cpp
    src/models/Workout.cpp
    src/models/Goal.cpp
    src/models/ModelFactory.cpp
    src/views/JsonView.cpp
    src/strategies/AuthStrategy.cpp
    src/services/AuthService.cpp
    src/services/NotificationService.cpp
    src/services/TaskQueue.cpp
)

target_include_directories(playsport PRIVATE src)

# Подключаем библиотеки
if(WIN32)
    target_link_libraries(playsport ${WINSOCK_LIB})
endif()

# Создаем папку public и копируем index.html
add_custom_command(TARGET playsport POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/public
    COMMENT "Creating public directory..."
)

# Копируем HTML файл если он существует
if(EXISTS ${CMAKE_SOURCE_DIR}/public/index.html)
    add_custom_command(TARGET playsport POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/public/index.html
            ${CMAKE_BINARY_DIR}/public/index.html
        COMMENT "Copying index.html..."
    )
endif()