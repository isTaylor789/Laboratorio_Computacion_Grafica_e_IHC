@echo off
REM ============================================================
REM  Previo 5 - Brazo robot jerarquico (OpenGL)
REM  Script de compilacion para WINDOWS
REM  Requisito: tener instalado CMake y Visual Studio 2022
REM             (con "Desktop development with C++")
REM  Dependencias C++ (una sola vez):
REM     vcpkg install glfw3 glew glm
REM  Uso:
REM     build.bat
REM     build.bat run      (compila y ejecuta)
REM     build.bat clean    (borra la carpeta build)
REM ============================================================
setlocal

if "%1"=="clean" (
    echo [INFO] Borrando carpeta build...
    rmdir /s /q build 2>nul
    echo [OK] Limpio.
    exit /b 0
)

where cmake >nul 2>nul
if errorlevel 1 (
    echo [ERROR] No se encontro 'cmake'. Instalalo desde https://cmake.org/download/
    echo         y marca la opcion "Add CMake to the system PATH".
    exit /b 1
)

REM Si existe VCPKG_ROOT, se usa su toolchain para encontrar glfw3/glew/glm solos
set TOOLCHAIN=
if defined VCPKG_ROOT (
    if exist "%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" (
        set TOOLCHAIN=-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
        echo [INFO] Usando vcpkg: %VCPKG_ROOT%
    )
)

echo [1/2] Configurando con CMake...
cmake -S . -B build %TOOLCHAIN%
if errorlevel 1 (
    echo [ERROR] Fallo la configuracion. Revisa que tengas Visual Studio + vcpkg instalados.
    echo         Prueba: vcpkg install glfw3 glew glm
    exit /b 1
)

echo [2/2] Compilando (Release)...
cmake --build build --config Release
if errorlevel 1 (
    echo [ERROR] Fallo la compilacion.
    exit /b 1
)

echo.
echo [OK] Listo. El ejecutable esta en: build\Release\Brazo.exe
echo     Controles: WASD+Q/E camara ^| R/F T/G Y/H U/J I/K O/L articulaciones ^| P reset ^| ESC salir
echo.

if "%1"=="run" (
    echo [INFO] Ejecutando...
    ".\build\Release\Brazo.exe"
)
endlocal
