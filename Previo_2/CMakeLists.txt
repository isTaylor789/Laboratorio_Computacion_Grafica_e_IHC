cmake_minimum_required(VERSION 3.20)

project(Practica_1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(result
    main.cpp
)

target_include_directories(result PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/include
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(result PRIVATE
    glfw
    GLEW::GLEW
    OpenGL::GL
)
