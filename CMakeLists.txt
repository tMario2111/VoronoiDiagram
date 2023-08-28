cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

project(voronoi_diagram)

add_executable(voronoi_diagram main.cpp
    src/App.hpp src/App.cpp
    src/Voronoi.hpp src/Voronoi.cpp)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)

target_link_libraries(voronoi_diagram PRIVATE
    sfml-main sfml-system sfml-graphics sfml-window
    imgui::imgui ImGui-SFML::ImGui-SFML)