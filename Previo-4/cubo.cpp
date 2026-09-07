#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera position
float camX = 0.0f;
float camY = 0.0f;
float camZ = 3.0f;

// Vertex Shader
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

// Fragment Shader
const char* fragmentShaderSource = R"(
#version 330 core

in vec3 ourColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
)";

void Inputs(GLFWwindow* window)
{
    // Close window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camX += 0.04f;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camX -= 0.04f;

    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        camY += 0.04f;

    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        camY -= 0.04f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camZ -= 0.04f;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camZ += 0.04f;
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cout << "Error initializing GLFW\n";
        return -1;
    }

    // OpenGL version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH,
        SCR_HEIGHT,
        "Cubo - Cambio de Perspectiva",
        nullptr,
        nullptr
    );

    if (window == nullptr)
    {
        std::cout << "Error creating GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;

    GLenum glewError = glewInit();

    if (glewError != GLEW_OK &&
        glewError != GLEW_ERROR_NO_GLX_DISPLAY)
    {
        std::cout << "GLEW error: "
                  << glewGetErrorString(glewError)
                  << "\n";
        return -1;
    }

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // -------------------------
    // Cube vertices
    // position: X Y Z
    // color:    R G B
    // -------------------------

    float vertices[] = {

        // FRONT - Orange
        -0.5f, -0.5f,  0.5f,  1.0f, 0.35f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.35f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.35f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.35f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.35f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.35f, 0.0f,

        // BACK - Purple
        -0.5f, -0.5f, -0.5f,  0.55f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.55f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.55f, 0.0f, 1.0f,

         0.5f,  0.5f, -0.5f,  0.55f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.55f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.55f, 0.0f, 1.0f,

        // RIGHT - Cyan
         0.5f, -0.5f,  0.5f,  0.0f, 0.85f, 0.85f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.85f, 0.85f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.85f, 0.85f,

         0.5f,  0.5f, -0.5f,  0.0f, 0.85f, 0.85f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.85f, 0.85f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.85f, 0.85f,

        // LEFT - Yellow
        -0.5f, -0.5f, -0.5f,  1.0f, 0.85f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.85f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.85f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.85f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.85f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.85f, 0.0f,

        // TOP - Green
        -0.5f,  0.5f,  0.5f,  0.1f, 0.9f, 0.2f,
         0.5f,  0.5f,  0.5f,  0.1f, 0.9f, 0.2f,
         0.5f,  0.5f, -0.5f,  0.1f, 0.9f, 0.2f,

         0.5f,  0.5f, -0.5f,  0.1f, 0.9f, 0.2f,
        -0.5f,  0.5f, -0.5f,  0.1f, 0.9f, 0.2f,
        -0.5f,  0.5f,  0.5f,  0.1f, 0.9f, 0.2f,

        // BOTTOM - Blue
        -0.5f, -0.5f, -0.5f,  0.15f, 0.3f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.15f, 0.3f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.15f, 0.3f, 1.0f,

         0.5f, -0.5f,  0.5f,  0.15f, 0.3f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.15f, 0.3f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.15f, 0.3f, 1.0f
    };

    // Create and compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Create and compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Create shader program
    unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Delete individual shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // VAO and VBO
    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    // Position
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    // Projection
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        (float)SCR_WIDTH / (float)SCR_HEIGHT,
        0.1f,
        100.0f
    );

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        Inputs(window);

        // Background
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // MODEL = identity
        // The cube itself does NOT rotate or move.
        glm::mat4 model = glm::mat4(1.0f);

        // VIEW = camera movement
        glm::mat4 view = glm::mat4(1.0f);

        view = glm::translate(
            view,
            glm::vec3(-camX, -camY, -camZ)
        );

        // Send matrices
        int modelLoc =
            glGetUniformLocation(shaderProgram, "model");

        int viewLoc =
            glGetUniformLocation(shaderProgram, "view");

        int projectionLoc =
            glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glUniformMatrix4fv(
            viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );

        glUniformMatrix4fv(
            projectionLoc,
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );

        // Draw cube
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
