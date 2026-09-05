cmake_minimum_required(VERSION 3.20)

project(OpenGLBoxes LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(glfw3 3.5 REQUIRED)
find_package(glm 1.0.3 REQUIRED)

add_library(glad
    external/glad/src/gl.c
)

target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
)

add_executable(OpenGLBoxes
    src/main.cpp
    src/Box.cpp
    src/Renderer.cpp
)

target_include_directories(OpenGLBoxes PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
)

target_link_libraries(OpenGLBoxes PRIVATE
    glfw
    glad
    glm::glm
)
