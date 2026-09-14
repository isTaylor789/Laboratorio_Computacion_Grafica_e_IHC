cmake_minimum_required(VERSION 3.21)

project(BrazoRobot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Brazo main.cpp)

# Headers locales (respaldo para Windows sin vcpkg / equipos de laboratorio)
target_include_directories(Brazo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/include
)

# --- Dependencias ---
# Linux (Arch/Ubuntu):  sudo pacman -S glfw glew glm  /  sudo apt install libglfw3-dev libglew-dev libglm-dev
# Windows (vcpkg):      vcpkg install glfw3 glew glm
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

# GLM es header-only: en Linux suele estar en /usr/include y no necesita link.
# En Windows (vcpkg) sí provee el target glm::glm; si existe, lo enlazamos.
find_package(glm CONFIG QUIET)

target_link_libraries(Brazo PRIVATE glfw GLEW::GLEW OpenGL::GL)
if(glm_FOUND)
    target_link_libraries(Brazo PRIVATE glm::glm)
endif()

# En Visual Studio: que "Run" encuentre los DLL de vcpkg y el exe quede en build/
if(WIN32)
    set_property(TARGET Brazo PROPERTY
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")
    # Copia los DLL del runtime (glfw3.dll, glew32.dll) junto al .exe tras compilar
    add_custom_command(TARGET Brazo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:Brazo>
            $<TARGET_FILE_DIR:Brazo>
        COMMAND_EXPAND_LISTS
    )
endif()
