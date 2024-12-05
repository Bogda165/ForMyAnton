cmake_minimum_required(VERSION 3.29)
project(guiPks)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(untitled6 main.cpp
        ChatApp.cpp
        ChatApp.h)
target_link_libraries(untitled6
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

set_target_properties(untitled6 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)