// =========================================================
// Previo 5 — Jerarquia de componentes en OpenGL (Brazo robot)
// ---------------------------------------------------------
// Jerarquia:
//
//   HOMBRO (pivote fijo)
//     -> BRAZO (verde)
//       -> CODO
//         -> ANTEBRAZO (rojo)
//           -> MUNECA
//             -> PALMA (blanca)
//               -> DEDO SUP (falange proximal cian)
//                 -> DEDO SUP (falange distal magenta)
//               -> DEDO INF (falange proximal cian)
//                 -> DEDO INF (falange distal magenta)
//
// Si se mueve el hombro, TODO el brazo lo sigue.
// Si se mueve el codo, antebrazo + muneca + dedos lo siguen, etc.
//
// Controles:
//   Camara (perspectiva) : W/S adelante/atras, A/D izq/der,
//                          Q/E o PageUp/PageDown arriba/abajo
//   Hombro pitch (arriba/abajo) : R / F   [-80,  80]
//   Hombro yaw   (lateral)       : T / G   [-70,  70]
//   Codo                        : Y / H   [  0, 135]
//   Muneca                      : U / J   [-70,  70]
//   Dedos base (abrir/cerrar)   : I / K   [  5,  55]
//   Dedos punta (doblar)        : O / L   [  0,  70]
//   Reset pose                  : P
//   Cerrar ventana              : ESC
// =========================================================

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>

// ---------------------------------------------------------
// Ventana
// ---------------------------------------------------------
const unsigned int SCR_WIDTH  = 1000;
const unsigned int SCR_HEIGHT = 700;

// ---------------------------------------------------------
// Estado: articulaciones (grados)
// ---------------------------------------------------------
float shoulderPitch = 10.0f;   // R / F  -> rotacion en Z (subir / bajar)
float shoulderYaw   = 0.0f;    // T / G  -> rotacion en Y (girar lateral)
float elbowAngle    = 20.0f;   // Y / H  -> codo
float wristAngle    = -10.0f;  // U / J  -> muneca
float fingerBase    = 22.0f;   // I / K  -> apertura falange proximal
float fingerTip     = 25.0f;   // O / L  -> doblez falange distal

// Limites (para delimitar el movimiento y no romper la malla)
const float SHOULDER_PITCH_MIN = -80.0f, SHOULDER_PITCH_MAX = 80.0f;
const float SHOULDER_YAW_MIN   = -70.0f, SHOULDER_YAW_MAX   = 70.0f;
const float ELBOW_MIN = 0.0f,   ELBOW_MAX = 135.0f;
const float WRIST_MIN = -70.0f, WRIST_MAX = 70.0f;
const float FBASE_MIN = 5.0f,   FBASE_MAX = 55.0f;
const float FTIP_MIN  = 0.0f,   FTIP_MAX  = 70.0f;

// ---------------------------------------------------------
// Estado: camara (perspectiva libre con WASD)
// ---------------------------------------------------------
float camX = 0.0f;
float camY = 2.0f;
float camZ = 7.0f;

static float clampf(float v, float lo, float hi)
{
    return std::max(lo, std::min(v, hi));
}

void printHelp()
{
    std::cout << "\n==============================================\n"
              << "  PREVIO 5 — Brazo robot jerarquico (OpenGL)\n"
              << "==============================================\n"
              << " Camara:\n"
              << "   W/S : adelante / atras\n"
              << "   A/D : izquierda / derecha\n"
              << "   Q/E o PageUp/PageDown : arriba / abajo\n"
              << " Brazo:\n"
              << "   R/F : Hombro arriba/abajo   [-80, 80]\n"
              << "   T/G : Hombro giro lateral   [-70, 70]\n"
              << "   Y/H : Codo                  [0, 135]\n"
              << "   U/J : Muneca                [-70, 70]\n"
              << "   I/K : Dedos base            [5, 55]\n"
              << "   O/L : Dedos punta           [0, 70]\n"
              << "   P   : Reset pose\n"
              << "   ESC : Cerrar ventana\n"
              << "==============================================\n\n";
}

// ---------------------------------------------------------
// Entradas (dt = delta time para movimiento suave)
// ---------------------------------------------------------
void Inputs(GLFWwindow* window, float dt)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // ---------- Camara ----------
    const float camSpeed = 3.0f * dt;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camZ -= camSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camZ += camSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camX -= camSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camX += camSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)   camY += camSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) camY -= camSpeed;

    camX = clampf(camX, -8.0f, 8.0f);
    camY = clampf(camY, 0.3f, 8.0f);
    camZ = clampf(camZ, 2.0f, 15.0f);

    // ---------- Articulaciones ----------
    const float jointSpeed = 60.0f * dt;   // grados por segundo
    const float fingerSpeed = 45.0f * dt;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        shoulderPitch = clampf(shoulderPitch + jointSpeed, SHOULDER_PITCH_MIN, SHOULDER_PITCH_MAX);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        shoulderPitch = clampf(shoulderPitch - jointSpeed, SHOULDER_PITCH_MIN, SHOULDER_PITCH_MAX);

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        shoulderYaw = clampf(shoulderYaw + jointSpeed, SHOULDER_YAW_MIN, SHOULDER_YAW_MAX);
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        shoulderYaw = clampf(shoulderYaw - jointSpeed, SHOULDER_YAW_MIN, SHOULDER_YAW_MAX);

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        elbowAngle = clampf(elbowAngle + jointSpeed, ELBOW_MIN, ELBOW_MAX);
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        elbowAngle = clampf(elbowAngle - jointSpeed, ELBOW_MIN, ELBOW_MAX);

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        wristAngle = clampf(wristAngle + jointSpeed, WRIST_MIN, WRIST_MAX);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        wristAngle = clampf(wristAngle - jointSpeed, WRIST_MIN, WRIST_MAX);

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        fingerBase = clampf(fingerBase + fingerSpeed, FBASE_MIN, FBASE_MAX);
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        fingerBase = clampf(fingerBase - fingerSpeed, FBASE_MIN, FBASE_MAX);

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        fingerTip = clampf(fingerTip + fingerSpeed, FTIP_MIN, FTIP_MAX);
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        fingerTip = clampf(fingerTip - fingerSpeed, FTIP_MIN, FTIP_MAX);

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        shoulderPitch = 10.0f;
        shoulderYaw   = 0.0f;
        elbowAngle    = 20.0f;
        wristAngle    = -10.0f;
        fingerBase    = 22.0f;
        fingerTip     = 25.0f;
    }
}

void framebuffer_size_callback(GLFWwindow*, int w, int h)
{
    glViewport(0, 0, w, h);
}

// ---------------------------------------------------------
// Shaders
// ---------------------------------------------------------
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;
uniform vec3 uColor;
uniform int uUseSolid;   // 1 = usar uColor, 0 = usar color por vertice
void main()
{
    if (uUseSolid == 1)
        FragColor = vec4(uColor, 1.0);
    else
        FragColor = vec4(ourColor, 1.0);
}
)";

unsigned int compileShader(unsigned int type, const char* src)
{
    unsigned int s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    int ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetShaderInfoLog(s, 512, nullptr, log);
        std::cout << "Shader error: " << log << "\n";
    }
    return s;
}

// ---------------------------------------------------------
// MAIN
// ---------------------------------------------------------
int main()
{
    printHelp();

    if (!glfwInit()) { std::cout << "Error initializing GLFW\n"; return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
        "Previo 5 — Brazo robot jerarquico | WASD camara | R/F T/G Y/H U/J I/K O/L | ESC salir",
        nullptr, nullptr);
    if (!window) { std::cout << "Error creating window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK && glewError != GLEW_ERROR_NO_GLX_DISPLAY) {
        std::cout << "GLEW error: " << glewGetErrorString(glewError) << "\n";
        return -1;
    }
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // ---------------- Cubo unitario (1x1x1 centrado) ----------------
    // Se reutiliza para TODAS las piezas: solo cambia model + uColor.
    float vertices[] = {
        // FRONT naranja
        -0.5f,-0.5f, 0.5f,  1.0f,0.35f,0.0f,   0.5f,-0.5f, 0.5f,  1.0f,0.35f,0.0f,
         0.5f, 0.5f, 0.5f,  1.0f,0.35f,0.0f,   0.5f, 0.5f, 0.5f,  1.0f,0.35f,0.0f,
        -0.5f, 0.5f, 0.5f,  1.0f,0.35f,0.0f,  -0.5f,-0.5f, 0.5f,  1.0f,0.35f,0.0f,
        // BACK morado
        -0.5f,-0.5f,-0.5f,  0.55f,0.0f,1.0f,   0.5f,-0.5f,-0.5f,  0.55f,0.0f,1.0f,
         0.5f, 0.5f,-0.5f,  0.55f,0.0f,1.0f,   0.5f, 0.5f,-0.5f,  0.55f,0.0f,1.0f,
        -0.5f, 0.5f,-0.5f,  0.55f,0.0f,1.0f,  -0.5f,-0.5f,-0.5f,  0.55f,0.0f,1.0f,
        // RIGHT cian
         0.5f,-0.5f, 0.5f,  0.0f,0.85f,0.85f,  0.5f,-0.5f,-0.5f,  0.0f,0.85f,0.85f,
         0.5f, 0.5f,-0.5f,  0.0f,0.85f,0.85f,  0.5f, 0.5f,-0.5f,  0.0f,0.85f,0.85f,
         0.5f, 0.5f, 0.5f,  0.0f,0.85f,0.85f,  0.5f,-0.5f, 0.5f,  0.0f,0.85f,0.85f,
        // LEFT amarillo
        -0.5f,-0.5f,-0.5f,  1.0f,0.85f,0.0f,  -0.5f,-0.5f, 0.5f,  1.0f,0.85f,0.0f,
        -0.5f, 0.5f, 0.5f,  1.0f,0.85f,0.0f,  -0.5f, 0.5f, 0.5f,  1.0f,0.85f,0.0f,
        -0.5f, 0.5f,-0.5f,  1.0f,0.85f,0.0f,  -0.5f,-0.5f,-0.5f,  1.0f,0.85f,0.0f,
        // TOP verde
        -0.5f, 0.5f, 0.5f,  0.1f,0.9f,0.2f,    0.5f, 0.5f, 0.5f,  0.1f,0.9f,0.2f,
         0.5f, 0.5f,-0.5f,  0.1f,0.9f,0.2f,    0.5f, 0.5f,-0.5f,  0.1f,0.9f,0.2f,
        -0.5f, 0.5f,-0.5f,  0.1f,0.9f,0.2f,   -0.5f, 0.5f, 0.5f,  0.1f,0.9f,0.2f,
        // BOTTOM azul
        -0.5f,-0.5f,-0.5f,  0.15f,0.3f,1.0f,   0.5f,-0.5f,-0.5f,  0.15f,0.3f,1.0f,
         0.5f,-0.5f, 0.5f,  0.15f,0.3f,1.0f,   0.5f,-0.5f, 0.5f,  0.15f,0.3f,1.0f,
        -0.5f,-0.5f, 0.5f,  0.15f,0.3f,1.0f,  -0.5f,-0.5f,-0.5f,  0.15f,0.3f,1.0f,
    };

    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glDeleteShader(vs); glDeleteShader(fs);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    int modelLoc      = glGetUniformLocation(shaderProgram, "model");
    int viewLoc       = glGetUniformLocation(shaderProgram, "view");
    int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    int colorLoc      = glGetUniformLocation(shaderProgram, "uColor");
    int solidLoc      = glGetUniformLocation(shaderProgram, "uUseSolid");

    // ---------------- Colores por pieza (como en las capturas) ----------------
    glm::vec3 cBrazo   (0.20f, 0.85f, 0.30f);  // verde
    glm::vec3 cAntebra (0.85f, 0.20f, 0.30f);  // rojo
    glm::vec3 cMuneca  (0.92f, 0.92f, 0.94f);  // blanco
    glm::vec3 cProx    (0.10f, 0.85f, 0.85f);  // cian (falange proximal)
    glm::vec3 cDist    (0.85f, 0.15f, 0.85f);  // magenta (falange distal)
    glm::vec3 cBase    (0.35f, 0.35f, 0.40f);  // gris (hombro / pedestal)
    glm::vec3 cSuelo   (0.16f, 0.16f, 0.19f);  // suelo

    // ---------------- Dimensiones ----------------
    const float brazoLen = 2.2f, brazoSec = 0.60f;
    const float anteLen  = 1.6f, anteSec  = 0.50f;
    const float palmLen  = 0.35f, palmSecY = 0.70f, palmSecZ = 0.55f;
    const float proxLen  = 0.75f, proxSec  = 0.18f;
    const float tipLen   = 0.75f, tipSec   = 0.16f;
    const float fingerOffY = 0.22f;   // separacion de los 2 dedos en Y
    const float fingerOffZ = 0.0f;
    const glm::vec3 shoulderPos(0.0f, 1.6f, 0.0f);

    auto drawBox = [&](const glm::mat4& model, const glm::vec3& color) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, color.r, color.g, color.b);
        glUniform1i(solidLoc, 1);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    };

    float lastTime = (float)glfwGetTime();

    // ---------------- Loop ----------------
    while (!glfwWindowShouldClose(window))
    {
        float now = (float)glfwGetTime();
        float dt = now - lastTime;
        lastTime = now;
        if (dt > 0.05f) dt = 0.05f; // evita saltos al arrastrar ventana

        Inputs(window, dt);

        glClearColor(0.05f, 0.05f, 0.07f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // Camara libre (WASD) mirando al centro del brazo
        glm::vec3 camPos(camX, camY, camZ);
        glm::vec3 target(0.0f, 1.3f, 0.0f);
        glm::mat4 view = glm::lookAt(camPos, target, glm::vec3(0.0f, 1.0f, 0.0f));
        int w, h; glfwGetFramebufferSize(window, &w, &h);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)w / (float)(h > 0 ? h : 1), 0.1f, 100.0f);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // ============ SUELO (referencia, no se mueve) ============
        {
            glm::mat4 m(1.0f);
            m = glm::translate(m, glm::vec3(0.0f, -0.05f, 0.0f));
            m = glm::scale(m, glm::vec3(20.0f, 0.1f, 20.0f));
            drawBox(m, cSuelo);
        }

        // ============ BASE / HOMBRO (pivote fijo) ============
        // Pedestal para que el hombro no flote
        {
            glm::mat4 m(1.0f);
            m = glm::translate(m, glm::vec3(shoulderPos.x - 0.25f, 0.75f, shoulderPos.z));
            m = glm::scale(m, glm::vec3(0.5f, 1.5f, 0.5f));
            drawBox(m, cBase);
        }
        // Nudo del hombro
        glm::mat4 shoulderBase = glm::translate(glm::mat4(1.0f), shoulderPos);

        // ============ JERARQUIA ============
        // Hombro: yaw (Y) * pitch (Z). Todo lo demas cuelga de aqui.
        glm::mat4 mShoulder =
            shoulderBase *
            glm::rotate(glm::mat4(1.0f), glm::radians(shoulderYaw),   glm::vec3(0,1,0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(shoulderPitch), glm::vec3(0,0,1));

        // ---- BRAZO (verde): se extiende en +X desde el hombro ----
        {
            glm::mat4 m = mShoulder *
                glm::translate(glm::mat4(1.0f), glm::vec3(brazoLen * 0.5f, 0, 0)) *
                glm::scale(glm::mat4(1.0f), glm::vec3(brazoLen, brazoSec, brazoSec));
            drawBox(m, cBrazo);
        }

        // ---- CODO (pivote al final del brazo) ----
        glm::mat4 mElbow = mShoulder *
            glm::translate(glm::mat4(1.0f), glm::vec3(brazoLen, 0, 0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(elbowAngle), glm::vec3(0,0,1));

        // ---- ANTEBRAZO (rojo) ----
        {
            glm::mat4 m = mElbow *
                glm::translate(glm::mat4(1.0f), glm::vec3(anteLen * 0.5f, 0, 0)) *
                glm::scale(glm::mat4(1.0f), glm::vec3(anteLen, anteSec, anteSec));
            drawBox(m, cAntebra);
        }

        // ---- MUNECA (pivote al final del antebrazo) ----
        glm::mat4 mWrist = mElbow *
            glm::translate(glm::mat4(1.0f), glm::vec3(anteLen, 0, 0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(wristAngle), glm::vec3(0,0,1));

        // ---- PALMA (blanca) ----
        {
            glm::mat4 m = mWrist *
                glm::translate(glm::mat4(1.0f), glm::vec3(palmLen * 0.5f, 0, 0)) *
                glm::scale(glm::mat4(1.0f), glm::vec3(palmLen, palmSecY, palmSecZ));
            drawBox(m, cMuneca);
        }

        // ---- DEDOS: 2 dedos, cada uno con 2 falanges (2 intersecciones) ----
        // Dedo superior (+Y) e inferior (-Y). La apertura es simetrica.
        for (int side = 0; side < 2; ++side)
        {
            float s = (side == 0) ? 1.0f : -1.0f;  // +1 arriba, -1 abajo

            // Falange proximal (cian): pivote en la palma + rotacion de apertura
            glm::mat4 mFingerBase = mWrist *
                glm::translate(glm::mat4(1.0f),
                    glm::vec3(palmLen, s * fingerOffY, fingerOffZ)) *
                glm::rotate(glm::mat4(1.0f), glm::radians(s * fingerBase), glm::vec3(0,0,1));

            {
                glm::mat4 m = mFingerBase *
                    glm::translate(glm::mat4(1.0f), glm::vec3(proxLen * 0.5f, 0, 0)) *
                    glm::scale(glm::mat4(1.0f), glm::vec3(proxLen, proxSec, proxSec));
                drawBox(m, cProx);
            }

            // Falange distal (magenta): pivote al final de la proximal.
            // Se dobla hacia ADENTRO (signo contrario a la apertura) para cerrar pinza.
            glm::mat4 mFingerTip = mFingerBase *
                glm::translate(glm::mat4(1.0f), glm::vec3(proxLen, 0, 0)) *
                glm::rotate(glm::mat4(1.0f), glm::radians(-s * fingerTip), glm::vec3(0,0,1));

            {
                glm::mat4 m = mFingerTip *
                    glm::translate(glm::mat4(1.0f), glm::vec3(tipLen * 0.5f, 0, 0)) *
                    glm::scale(glm::mat4(1.0f), glm::vec3(tipLen, tipSec, tipSec));
                drawBox(m, cDist);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
