#include <iostream>
#include <vector>
#include <cmath>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// ============================================================
// WINDOW
// ============================================================

const unsigned int WIDTH = 1000;
const unsigned int HEIGHT = 700;


// ============================================================
// SHADERS
// ============================================================

const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 color;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    color = aColor;
}
)";


const char* fragmentShaderSource = R"(
#version 330 core

in vec3 color;

out vec4 FragColor;

void main()
{
    FragColor = vec4(color, 1.0);
}
)";


// ============================================================
// SHADER COMPILATION
// ============================================================

unsigned int createShaderProgram()
{
    // -------------------------
    // Vertex shader
    // -------------------------

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        nullptr
    );

    glCompileShader(vertexShader);


    // Check compilation
    int success;
    char infoLog[512];

    glGetShaderiv(
        vertexShader,
        GL_COMPILE_STATUS,
        &success
    );

    if (!success)
    {
        glGetShaderInfoLog(
            vertexShader,
            512,
            nullptr,
            infoLog
        );

        std::cout
            << "Vertex shader error:\n"
            << infoLog
            << std::endl;
    }


    // -------------------------
    // Fragment shader
    // -------------------------

    unsigned int fragmentShader =
        glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        nullptr
    );

    glCompileShader(fragmentShader);


    // Check compilation

    glGetShaderiv(
        fragmentShader,
        GL_COMPILE_STATUS,
        &success
    );

    if (!success)
    {
        glGetShaderInfoLog(
            fragmentShader,
            512,
            nullptr,
            infoLog
        );

        std::cout
            << "Fragment shader error:\n"
            << infoLog
            << std::endl;
    }


    // -------------------------
    // Shader program
    // -------------------------

    unsigned int program =
        glCreateProgram();

    glAttachShader(
        program,
        vertexShader
    );

    glAttachShader(
        program,
        fragmentShader
    );

    glLinkProgram(program);


    // Check linking

    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &success
    );

    if (!success)
    {
        glGetProgramInfoLog(
            program,
            512,
            nullptr,
            infoLog
        );

        std::cout
            << "Shader linking error:\n"
            << infoLog
            << std::endl;
    }


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}


// ============================================================
// BOX
// ============================================================

struct Box
{
    glm::vec3 position;

    glm::vec3 rotation;

    glm::vec3 scale;
};


// ============================================================
// CAMERA
// ============================================================

glm::vec3 cameraPosition =
    glm::vec3(6.0f, 5.0f, 8.0f);

glm::vec3 cameraTarget =
    glm::vec3(0.0f, 2.0f, 0.0f);


// ============================================================
// MAIN
// ============================================================

int main()
{
    // ========================================================
    // INITIALIZE GLFW
    // ========================================================

    if (!glfwInit())
    {
        std::cout
            << "Failed to initialize GLFW"
            << std::endl;

        return -1;
    }


    // Tell GLFW which OpenGL version we want

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3
    );

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );


    // ========================================================
    // CREATE WINDOW
    // ========================================================

    GLFWwindow* window =
        glfwCreateWindow(
            WIDTH,
            HEIGHT,
            "OpenGL - 4 Stacked Boxes",
            nullptr,
            nullptr
        );

    if (!window)
    {
        std::cout
            << "Failed to create GLFW window"
            << std::endl;

        glfwTerminate();

        return -1;
    }


    glfwMakeContextCurrent(window);


    // ========================================================
    // INITIALIZE GLAD
    // ========================================================

    int version = gladLoadGL(
        (GLADloadfunc)glfwGetProcAddress
    );

    if (!version)
    {
        std::cout
            << "Failed to initialize GLAD"
            << std::endl;

        return -1;
    }


    std::cout
        << "OpenGL version: "
        << glGetString(GL_VERSION)
        << std::endl;


    // ========================================================
    // VIEWPORT
    // ========================================================

    glViewport(
        0,
        0,
        WIDTH,
        HEIGHT
    );


    // ========================================================
    // DEPTH TEST
    // ========================================================

    glEnable(GL_DEPTH_TEST);


    // ========================================================
    // CREATE SHADER
    // ========================================================

    unsigned int shaderProgram =
        createShaderProgram();


    // ========================================================
    // CUBE DATA
    // ========================================================

    float vertices[] =
    {
        // Position              // Color

        // Front
        -0.5f, -0.5f,  0.5f,      1.0f, 0.2f, 0.2f,
         0.5f, -0.5f,  0.5f,      0.2f, 1.0f, 0.2f,
         0.5f,  0.5f,  0.5f,      0.2f, 0.4f, 1.0f,
        -0.5f,  0.5f,  0.5f,      1.0f, 0.2f, 1.0f,

        // Back
        -0.5f, -0.5f, -0.5f,      1.0f, 0.5f, 0.2f,
         0.5f, -0.5f, -0.5f,      0.2f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 0.2f,
        -0.5f,  0.5f, -0.5f,      0.8f, 0.2f, 0.2f
    };


    unsigned int indices[] =
    {
        // Front
        0, 1, 2,
        2, 3, 0,

        // Back
        4, 5, 6,
        6, 7, 4,

        // Left
        0, 3, 7,
        7, 4, 0,

        // Right
        1, 5, 6,
        6, 2, 1,

        // Bottom
        0, 4, 5,
        5, 1, 0,

        // Top
        3, 2, 6,
        6, 7, 3
    };


    // ========================================================
    // BUFFERS
    // ========================================================

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    glGenVertexArrays(
        1,
        &VAO
    );

    glGenBuffers(
        1,
        &VBO
    );

    glGenBuffers(
        1,
        &EBO
    );


    // ========================================================
    // VAO
    // ========================================================

    glBindVertexArray(VAO);


    // ========================================================
    // VBO
    // ========================================================

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );


    // ========================================================
    // EBO
    // ========================================================

    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        EBO
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW
    );


    // ========================================================
    // POSITION ATTRIBUTE
    // ========================================================

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);


    // ========================================================
    // COLOR ATTRIBUTE
    // ========================================================

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);


    // ========================================================
    // UNBIND
    // ========================================================

    glBindBuffer(
        GL_ARRAY_BUFFER,
        0
    );

    glBindVertexArray(0);


    // ========================================================
    // CREATE 4 BOXES
    // ========================================================

    std::vector<Box> boxes =
    {
        // Cubo 1 - abajo
        {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, -5.0f),
            glm::vec3(1.5f, 1.5f, 1.5f)
        },

        // Cubo 2 - ligeramente a la derecha
        {
            glm::vec3(0.35f, 1.45f, 0.0f),
            glm::vec3(0.0f, 0.0f, 8.0f),
            glm::vec3(1.5f, 1.5f, 1.5f)
        },

        // Cubo 3 - ligeramente a la izquierda
        {
            glm::vec3(-0.25f, 2.90f, 0.0f),
            glm::vec3(0.0f, 0.0f, -7.0f),
            glm::vec3(1.5f, 1.5f, 1.5f)
        },

        // Cubo 4 - ligeramente a la derecha
        {
            glm::vec3(0.30f, 4.35f, 0.0f),
            glm::vec3(0.0f, 0.0f, 6.0f),
            glm::vec3(1.5f, 1.5f, 1.5f)
        }
    };


    // ========================================================
    // MATRICES
    // ========================================================

    glm::mat4 projection =
        glm::perspective(
            glm::radians(45.0f),
            (float)WIDTH / (float)HEIGHT,
            0.1f,
            100.0f
        );


    glm::mat4 view =
        glm::lookAt(
            cameraPosition,
            cameraTarget,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );


    // ========================================================
    // GET UNIFORM LOCATIONS
    // ========================================================

    int modelLocation =
        glGetUniformLocation(
            shaderProgram,
            "model"
        );

    int viewLocation =
        glGetUniformLocation(
            shaderProgram,
            "view"
        );

    int projectionLocation =
        glGetUniformLocation(
            shaderProgram,
            "projection"
        );


    // ========================================================
    // SEND VIEW + PROJECTION
    // ========================================================

    glUseProgram(shaderProgram);

    glUniformMatrix4fv(
        viewLocation,
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        projectionLocation,
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );


    // ========================================================
    // RENDER LOOP
    // ========================================================

    while (!glfwWindowShouldClose(window))
    {
        // ----------------------------------------------------
        // Clear screen
        // ----------------------------------------------------

        glClearColor(
            0.08f,
            0.08f,
            0.10f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );


        // ----------------------------------------------------
        // Use shader
        // ----------------------------------------------------

        glUseProgram(shaderProgram);


        // ----------------------------------------------------
        // Draw boxes
        // ----------------------------------------------------

        glBindVertexArray(VAO);


        for (const Box& box : boxes)
        {
            glm::mat4 model =
                glm::mat4(1.0f);


            // Position

            model =
                glm::translate(
                    model,
                    box.position
                );


            // Rotation X

            model =
                glm::rotate(
                    model,
                    glm::radians(box.rotation.x),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                );


            // Rotation Y

            model =
                glm::rotate(
                    model,
                    glm::radians(box.rotation.y),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );


            // Rotation Z

            model =
                glm::rotate(
                    model,
                    glm::radians(box.rotation.z),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                );


            // Scale

            model =
                glm::scale(
                    model,
                    box.scale
                );


            glUniformMatrix4fv(
                modelLocation,
                1,
                GL_FALSE,
                glm::value_ptr(model)
            );


            glDrawElements(
                GL_TRIANGLES,
                36,
                GL_UNSIGNED_INT,
                0
            );
        }


        // ----------------------------------------------------
        // Show frame
        // ----------------------------------------------------

        glfwSwapBuffers(window);

        glfwPollEvents();
    }


    // ========================================================
    // CLEANUP
    // ========================================================

    glDeleteVertexArrays(
        1,
        &VAO
    );

    glDeleteBuffers(
        1,
        &VBO
    );

    glDeleteBuffers(
        1,
        &EBO
    );

    glDeleteProgram(
        shaderProgram
    );


    glfwDestroyWindow(window);

    glfwTerminate();


    return 0;
}
