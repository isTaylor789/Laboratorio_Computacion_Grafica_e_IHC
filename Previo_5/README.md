# Previo 5 — Modelado jerárquico: brazo robot articulado en OpenGL

> **UNAM · Laboratorio de Computación Gráfica e Interacción Humano-Computadora**
> Lenguaje: **C++17** · API: **OpenGL 3.3 Core** · Ventaneo: **GLFW** · Cargador: **GLEW** · Matemáticas: **GLM**
> Autores: práctica de modelado jerárquico (hombro → brazo → codo → antebrazo → muñeca → dedos con 2 falanges).

---

## Índice

1. [Objetivo](#1-objetivo)
2. [Marco teórico](#2-marco-teórico)
3. [Descripción del modelo](#3-descripción-del-modelo)
4. [Movimiento: articulaciones y cámara (explicación completa)](#4-movimiento-articulaciones-y-cámara-explicación-completa)
5. [Implementación](#5-implementación)
6. [Compilación y ejecución](#6-compilación-y-ejecución)
7. [Estructura del proyecto](#7-estructura-del-proyecto)
8. [Resultados esperados](#8-resultados-esperados)
9. [Conclusiones](#9-conclusiones)
10. [Referencias](#10-referencias)

---

## 1. Objetivo

Construir un **brazo robot articulado** con **modelado jerárquico** en OpenGL, donde cada eslabón hereda la
transformación de su padre, de modo que:

- Si se mueve el **hombro**, se mueve **todo el brazo**.
- Si se mueve el **codo**, se mueven **antebrazo + muñeca + dedos**, pero no el brazo.
- Si se mueve la **muñeca**, se mueven **palma + dedos**.
- Si se mueve la **falange proximal**, la **distal la sigue** (2 intersecciones por dedo).

Además se delimitan los rangos articulares para no romper la malla, se colorea cada pieza como en la
referencia (verde / rojo / blanco / cian / magenta) y la cámara se controla en perspectiva con el teclado.

---

## 2. Marco teórico

### 2.1 Modelado jerárquico y grafo de escena

Un modelo articulado se representa como un **árbol de transformaciones** (grafo de escena). Cada nodo guarda
su transformación **local** (respecto a su padre) y su transformación **global** se obtiene acumulando:

```
M_global(hijo) = M_global(padre) · M_local(hijo)
```

Esto garantiza la propiedad fundamental del previo: **el hijo sigue al padre**.
En el programa esto se escribe directamente con GLM, por ejemplo:

```cpp
glm::mat4 mElbow = mShoulder
    * glm::translate(glm::mat4(1.0f), glm::vec3(brazoLen, 0, 0))
    * glm::rotate(glm::mat4(1.0f), glm::radians(elbowAngle), glm::vec3(0,0,1));
```

### 2.2 Matrices homogéneas y cadena MVP

Cada pieza parte de un **cubo unitario** centrado en el origen y se coloca con:

```
M_model = T(pivote) · R(θ) · T(offset) · S(dimensiones)
```

- `S` le da el tamaño (brazo largo, dedo delgado…),
- `T(offset)` lo recorre medio largo para que **el pivote quede en el extremo** (la articulación),
  no en el centro,
- `R(θ)` es la rotación articular controlada por teclado,
- `T(pivote)` lo cuelga del padre.

El vértice final se calcula en el *vertex shader* con la cadena clásica:

```
gl_Position = P (proyección) · V (vista/cámara) · M (modelo) · vec4(pos, 1)
```

### 2.3 Pivotes

El error más común es rotar el cubo sobre su centro (se ve como “tambaleo”). La solución usada aquí es el
patrón **trasladar medio largo → escalar**:

```cpp
mBrazo = mHombro
       * translate(+brazoLen/2 en X)   // el origen queda en el hombro
       * scale(brazoLen, seccion, seccion);
```

Así la rotación ocurre en la articulación, como un hueso real.

---

## 3. Descripción del modelo

### 3.1 Árbol jerárquico

```
HOMBRO (pivote fijo en [0, 1.6, 0], sobre pedestal)
└── BRAZO (verde)
    └── CODO (pivote al final del brazo)
        └── ANTEBRAZO (rojo)
            └── MUÑECA (pivote al final del antebrazo)
                └── PALMA (blanca)
                    ├── DEDO SUP · falange proximal (cian)
                    │   └── DEDO SUP · falange distal (magenta)
                    └── DEDO INF · falange proximal (cian)
                        └── DEDO INF · falange distal (magenta)
```

Total: **8 piezas móviles** (brazo, antebrazo, palma, 2 proximales, 2 distales) + pedestal + suelo + nudo
del hombro. Los 2 dedos cumplen el requisito de **2 intersecciones** (2 articulaciones en serie por dedo).

### 3.2 Piezas, dimensiones y colores

| Pieza | Color RGB | Dimensiones (X, Y, Z) | Función |
|---|---|---|---|
| Brazo | verde `(0.20, 0.85, 0.30)` | `2.20 × 0.60 × 0.60` | Eslabón del hombro |
| Antebrazo | rojo `(0.85, 0.20, 0.30)` | `1.60 × 0.50 × 0.50` | Eslabón del codo |
| Palma / muñeca | blanco `(0.92, 0.92, 0.94)` | `0.35 × 0.70 × 0.55` | Eslabón de la muñeca |
| Falange proximal ×2 | cian `(0.10, 0.85, 0.85)` | `0.75 × 0.18 × 0.18` | 1.ª intersección del dedo |
| Falange distal ×2 | magenta `(0.85, 0.15, 0.85)` | `0.75 × 0.16 × 0.16` | 2.ª intersección del dedo |
| Pedestal + suelo | gris | — | Referencia fija (no se articula) |

Los colores reproducen las capturas de referencia (`scope/`). Para cambiarlos basta editar las variables
`cBrazo, cAntebra, cMuneca, cProx, cDist` en `main.cpp`.

---

## 4. Movimiento: articulaciones y cámara (explicación completa)

### 4.1 Idea general

Hay **6 grados de libertad** articulares + **3 de cámara**. Cada tecla **suma o resta grados por segundo**
(`60 °/s` en articulaciones grandes, `45 °/s` en dedos) multiplicados por `dt` (*delta time*), de modo que el
movimiento es suave e independiente de los FPS:

```cpp
float dt = now - lastTime;                 // segundos desde el frame anterior
shoulderPitch = clamp(shoulderPitch + 60.0f * dt, -80.0f, +80.0f);
```

La función `clampf(v, min, max)` **delimita** el movimiento: si el usuario mantiene la tecla, el ángulo se
detiene en el tope en lugar de girar 360° y deformar el modelo.

### 4.2 Tabla completa de controles

**Cámara en perspectiva** (traslación del observador; siempre mira al punto `(0, 1.3, 0)`):

| Tecla | Acción | Rango |
|---|---|---|
| `W` / `S` | Cámara adelante / atrás (eje Z) | Z ∈ [2, 15] |
| `A` / `D` | Cámara izquierda / derecha (eje X) | X ∈ [−8, 8] |
| `Q` / `E` o `PageUp` / `PageDown` | Cámara arriba / abajo (eje Y) | Y ∈ [0.3, 8] |
| `ESC` | Cerrar ventana | — |

**Articulaciones** (rotaciones jerárquicas; `P` restaura la pose inicial):

| Articulación | Teclas | Eje | Límite | Qué arrastra |
|---|---|---|---|---|
| Hombro *pitch* (subir/bajar) | `R` / `F` | Z | **[−80°, +80°]** | Todo el brazo |
| Hombro *yaw* (giro lateral) | `T` / `G` | Y | **[−70°, +70°]** | Todo el brazo |
| Codo | `Y` / `H` | Z | **[0°, 135°]** | Antebrazo + muñeca + dedos |
| Muñeca | `U` / `J` | Z | **[−70°, +70°]** | Palma + dedos |
| Dedos base (apertura) | `I` / `K` | Z simétrico ± | **[5°, 55°]** | Falanges proximales + distales |
| Dedos punta (doblez pinza) | `O` / `L` | Z hacia adentro | **[0°, 70°]** | Falanges distales |
| Reset | `P` | — | — | Restaura `10°, 0°, 20°, −10°, 22°, 25°` |

### 4.3 Por qué esos límites

- **Hombro ±80° / ±70°**: permite levantar y girar el brazo sin que atraviese el pedestal ni dé la vuelta completa.
- **Codo 0°–135°**: el codo humano no hiperextiende (mínimo 0°) y su flexión máxima real ronda los 135°–145°.
- **Muñeca ±70°**: rango fisiológico aproximado de flexo-extensión sin dislocar la palma.
- **Base 5°–55°**: mínimo 5° para que los dedos no se interpenetren cerrados; máximo 55° para no abrir la pinza de más.
- **Punta 0°–70°**: 0° = dedo recto, 70° = pinza bien cerrada.

### 4.4 Los dedos (las “2 intersecciones”)

Cada dedo es una **cadena de 2 eslabones**:

1. La **proximal** nace en la palma, separada `±0.22` en Y (dedo superior / inferior) y rota `±fingerBase`
   (apertura simétrica: el de arriba abre hacia arriba, el de abajo hacia abajo).
2. La **distal** nace al final de la proximal y rota `∓fingerTip` (**signo contrario**: hacia adentro),
   de modo que al aumentar `fingerTip` la pinza se **cierra** en lugar de abrirse más.

### 4.5 La cámara

No es una cámara orbital con mouse: es una **cámara libre por teclado** implementada con:

```cpp
view = glm::lookAt(camPos, vec3(0, 1.3, 0), vec3(0, 1, 0));
```

`WASD` mueve `camPos` en el plano XZ y `Q/E` en Y, con sus propios *clamps* para no meterse bajo el suelo
ni alejarse de más. La proyección es perspectiva a 45° (`glm::perspective`), por eso se pide “perspectiva
de la cámara con WASD”.

---

## 5. Implementación

- **Un solo cubo, 36 vértices** (posición + color por cara). Todas las piezas lo reutilizan; solo cambian
  la matriz `model` y el color sólido. Esto minimiza memoria y *draw calls*.
- **Color por pieza** con uniforms `uColor` + `uUseSolid`: si `uUseSolid == 1` el *fragment shader* ignora
  el color por vértice y pinta toda la pieza del color de la articulación.
- **`drawBox(model, color)`**: lambda que sube `model` + `uColor` y dibuja. Cada pieza = 1 llamada.
- **Profundidad** con `glEnable(GL_DEPTH_TEST)` y limpieza por frame de color + profundidad.
- **Shaders GLSL 330 core** embebidos como *raw strings*; compilación con revisión de errores.
- **Callback de resize** (`framebuffer_size_callback`) para mantener el *viewport* y el *aspect ratio*.
- **Consola de ayuda**: al arrancar se imprimen todos los controles.

---

## 6. Compilación y ejecución

### 6.1 Dependencias

| Sistema | Instalación |
|---|---|
| Arch Linux | `sudo pacman -S glfw glew glm cmake base-devel` |
| Ubuntu/Debian | `sudo apt install libglfw3-dev libglew-dev libglm-dev cmake build-essential` |
| Windows | Instalar **CMake** + **Visual Studio 2022** (carga “Desktop development with C++”) y una vez: `vcpkg install glfw3 glew glm` |

### 6.2 Linux (script `run.sh`)

```bash
chmod +x run.sh
./run.sh
```

Equivale a:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/Brazo
```

### 6.3 Windows (script `build.bat`)

```bat
build.bat        :: solo compila  -> build\Release\Brazo.exe
build.bat run    :: compila y ejecuta
build.bat clean  :: borra build\
```

Si tienes `VCPKG_ROOT` definido, el script usa automáticamente su *toolchain* para encontrar
`glfw3/glew/glm` sin configurar nada más. Tras compilar, CMake copia los DLL (`glfw3.dll`,
`glew32.dll`) junto al `.exe`, así el programa es **portable dentro de la carpeta `build\Release`**.

### 6.4 Notas de portabilidad

- El `CMakeLists.txt` es el **mismo** en ambos sistemas: detecta `OpenGL`, `glfw3`, `GLEW` y, si existe,
  el target `glm::glm` (vcpkg); en Linux GLM llega por include del sistema.
- La carpeta `libs/include/` se conserva como respaldo de headers para equipos de laboratorio.
- El estándar es **C++17** en ambas plataformas; el ejecutable se llama `Brazo` (Linux) / `Brazo.exe` (Windows).

---

## 7. Estructura del proyecto

```
Previo_5/
├── main.cpp        # Todo el programa: ventana, shaders, jerarquía, entrada
├── CMakeLists.txt  # Build portable Linux + Windows (glfw/GLEW/GLM/OpenGL)
├── run.sh          # Compila y ejecuta en Linux
├── build.bat       # Compila (y opcionalmente ejecuta) en Windows
├── libs/include/   # Headers locales de respaldo (GL, GLFW)
├── scope/          # Capturas de referencia del modelo esperado
└── build/          # Generado por CMake (no versionar)
```

---

## 8. Resultados esperados

1. Al abrir, el brazo aparece **horizontal** (verde–rojo–blanco–cian–magenta) sobre fondo oscuro con suelo gris.
2. `R/F` levanta/baja **todo** el brazo desde el hombro; `T/G` lo gira lateralmente.
3. `Y/H` dobla solo el codo (el brazo verde queda fijo); `U/J` orienta la pinza.
4. `I/K` abre/cierra la base de los dedos; `O/L` dobla las puntas hacia adentro (pinza).
5. `WASD+Q/E` mueve la cámara sin mover el brazo; `P` restaura la pose; `ESC` cierra.
6. Ninguna articulación supera sus topes aunque se mantenga la tecla.

---

## 9. Conclusiones

- El modelado jerárquico reduce un brazo de 8 piezas a **6 ángulos**: cada nivel hereda el anterior,
  lo que equivale a una cinemática directa explícita.
- Colocar el **pivote en el extremo** (trasladar medio largo antes de escalar) es lo que hace que las
  rotaciones parezcan articulaciones reales.
- Delimitar con `clamp` + explicar los límites con criterio anatómico convierte un demo en un modelo
  defendible académicamente.
- Un solo `CMakeLists.txt` + dos scripts (`run.sh` / `build.bat`) hacen la práctica **portable**
  Linux ↔ Windows sin cambiar código.

---

## 10. Referencias

- Hearn, D. & Baker, M. P. — *Computer Graphics with OpenGL* (cap. modelado jerárquico / grafos de escena).
- OpenGL Tutorial / LearnOpenGL — *Transformations*, *Coordinate Systems*, *Camera*.
- Documentación de GLM: `translate / rotate / scale / perspective / lookAt`.
- Documentación de GLFW (entrada por teclado, ventanas, contexto OpenGL 3.3 Core).
- Capturas de referencia en `scope/` (modelo esperado del enunciado).
