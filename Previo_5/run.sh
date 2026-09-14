#!/bin/bash
# Previo 5 — compila y ejecuta el brazo robot jerarquico
set -e
cd "$(dirname "$0")"

echo "==> Configurando con CMake..."
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

echo "==> Compilando..."
cmake --build build -j"$(nproc)"

echo "==> Ejecutando ./build/Brazo"
echo "    WASD+Q/E: camara | R/F T/G Y/H U/J I/K O/L: articulaciones | P: reset | ESC: salir"
./build/Brazo
