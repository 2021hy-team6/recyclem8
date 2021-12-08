cmake_minimum_required(VERSION 3.11)

project(recycleAssistant VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

find_package(Threads REQUIRED)
find_package(OpenCV 4 REQUIRED)
find_package(rapidJSON REQUIRED MODULE)
find_library(PQ_LIB pq REQUIRED)
find_library(PQXX_LIB pqxx REQUIRED)

set(PROJECT_SOURCES
    source/main.cpp

    include/opencv_handler.h source/opencv_handler.cpp
    include/confighandler.h source/confighandler.cpp
    include/classifier.h source/classifier.cpp
    include/databasehandler.h source/databasehandler.cpp

    include/recyclem8.h source/recyclem8.cpp ui/recyclem8.ui
    include/settingsmenu.h source/settingsmenu.cpp ui/settingsmenu.ui
    include/statisticsmenu.h source/statisticsmenu.cpp ui/statisticsmenu.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE include ${OpenCV_INCLUDE_DIRS} ${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets rapidJSON::rapidJSON Threads::Threads ${OpenCV_LIBS} ${PostgreSQL_LIBRARIES} ${PQXX_LIB} ${PQ_LIB})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
