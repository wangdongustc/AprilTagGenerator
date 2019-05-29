cmake_minimum_required(VERSION 3.0.0)

project(AprilTagGenerator VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Svg Gui)

find_package(apriltags REQUIRED)

add_executable(${PROJECT_NAME} main.cpp
                               mainwindow.ui
                               mainwindow.h
                               mainwindow.cpp
                               tagpainter.h
                               tagpainter.cpp
                               )

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} Qt5::Widgets Qt5::Svg Qt5::Gui apriltags)
