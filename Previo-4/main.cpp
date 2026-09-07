#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

// ---------------------------------------------------------
// WINDOW
// ---------------------------------------------------------

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// ---------------------------------------------------------
// TABLE MOVEMENT
// ---------------------------------------------------------

float tableX = 0.0f;
float tableY = 0.0f;
float tableZ = 0.0f;


// ---------------------------------------------------------
// INPUTS
// ---------------------------------------------------------

void Inputs(GLFWwindow* window)
{
    // ESC = close
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // A / D = left / right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        tableX -= 0.03f;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        tableX += 0.03f;

    // W / S = forward / backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        tableZ -= 0.03f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        tableZ += 0.03f;

    // PAGE UP / PAGE DOWN = up / down
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        tableY += 0.03f;

    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        tableY -= 0.03f;
}


// ---------------------------------------------------------
// VERTEX SHADER
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
    gl_Position =
        projection *
        view *
        model *
        vec4(aPos, 1.0);

    ourColor = aColor;
}
)";


// ---------------------------------------------------------
// FRAGMENT SHADER
// ---------------------------------------------------------

const char* fragmentShaderSource = R"(
#version 330 core

in vec3 ourColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
)";


// ---------------------------------------------------------
// MAIN
// ---------------------------------------------------------

int main()
{
    // -----------------------------------------------------
    // INITIALIZE GLFW
    // -----------------------------------------------------

    if (!glfwInit())
    {
        std::cout << "Error initializing GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // -----------------------------------------------------
    // CREATE WINDOW
    // -----------------------------------------------------

    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH,
        SCR_HEIGHT,
        "Mesa - Computacion Grafica",
        nullptr,
        nullptr
    );

    if (window == nullptr)
    {
        std::cout << "Error creating window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);


    // -----------------------------------------------------
    // INITIALIZE GLEW
    // -----------------------------------------------------

    glewExperimental = GL_TRUE;

    GLenum glewError = glewInit();

    // Arch Linux + Wayland can return this even though
    // OpenGL itself is working correctly.
    if (glewError != GLEW_OK &&
        glewError != GLEW_ERROR_NO_GLX_DISPLAY)
    {
        std::cout << "GLEW error: "
                  << glewGetErrorString(glewError)
                  << "\n";

        return -1;
    }


    // -----------------------------------------------------
    // CHECK OPENGL
    // -----------------------------------------------------

    std::cout << "OpenGL: "
              << glGetString(GL_VERSION)
              << "\n";


    // -----------------------------------------------------
    // OPENGL SETTINGS
    // -----------------------------------------------------

    glEnable(GL_DEPTH_TEST);

    glViewport(
        0,
        0,
        SCR_WIDTH,
        SCR_HEIGHT
    );


    // -----------------------------------------------------
    // CUBE GEOMETRY
    // -----------------------------------------------------
    //
    // We use ONE cube and transform it into:
    //
    // 1 tabletop
    // 4 legs
    //
    // Every face has a different color.
    //
    // -----------------------------------------------------

    float vertices[] =
    {
        // FRONT - Orange
        -0.5f, -0.5f,  0.5f,   1.0f, 0.35f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 0.35f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 0.35f, 0.0f,

         0.5f,  0.5f,  0.5f,   1.0f, 0.35f, 0.0f,
        -0.5f,  0.5f,  0.5f,   1.0f, 0.35f, 0.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 0.35f, 0.0f,


        // BACK - Purple
        -0.5f, -0.5f, -0.5f,   0.55f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   0.55f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   0.55f, 0.0f, 1.0f,

         0.5f,  0.5f, -0.5f,   0.55f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.55f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.55f, 0.0f, 1.0f,


        // RIGHT - Cyan
         0.5f, -0.5f,  0.5f,   0.0f, 0.85f, 0.85f,
         0.5f, -0.5f, -0.5f,   0.0f, 0.85f, 0.85f,
         0.5f,  0.5f, -0.5f,   0.0f, 0.85f, 0.85f,

         0.5f,  0.5f, -0.5f,   0.0f, 0.85f, 0.85f,
         0.5f,  0.5f,  0.5f,   0.0f, 0.85f, 0.85f,
         0.5f, -0.5f,  0.5f,   0.0f, 0.85f, 0.85f,


        // LEFT - Yellow
        -0.5f, -0.5f, -0.5f,   1.0f, 0.85f, 0.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 0.85f, 0.0f,
        -0.5f,  0.5f,  0.5f,   1.0f, 0.85f, 0.0f,

        -0.5f,  0.5f,  0.5f,   1.0f, 0.85f, 0.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 0.85f, 0.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 0.85f, 0.0f,


        // TOP - Green
        -0.5f,  0.5f,  0.5f,   0.1f, 0.9f, 0.2f,
         0.5f,  0.5f,  0.5f,   0.1f, 0.9f, 0.2f,
         0.5f,  0.5f, -0.5f,   0.1f, 0.9f, 0.2f,

         0.5f,  0.5f, -0.5f,   0.1f, 0.9f, 0.2f,
        -0.5f,  0.5f, -0.5f,   0.1f, 0.9f, 0.2f,
        -0.5f,  0.5f,  0.5f,   0.1f, 0.9f, 0.2f,


        // BOTTOM - Blue
        -0.5f, -0.5f, -0.5f,   0.15f, 0.3f, 1.0f,
         0.5f, -0.5f, -0.5f,   0.15f, 0.3f, 1.0f,
         0.5f, -0.5f,  0.5f,   0.15f, 0.3f, 1.0f,

         0.5f, -0.5f,  0.5f,   0.15f, 0.3f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.15f, 0.3f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.15f, 0.3f, 1.0f
    };


    // -----------------------------------------------------
    // CREATE SHADERS
    // -----------------------------------------------------

    unsigned int vertexShader =
        glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        nullptr
    );

    glCompileShader(vertexShader);


    unsigned int fragmentShader =
        glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        nullptr
    );

    glCompileShader(fragmentShader);


    // -----------------------------------------------------
    // CREATE SHADER PROGRAM
    // -----------------------------------------------------

    unsigned int shaderProgram =
        glCreateProgram();

    glAttachShader(
        shaderProgram,
        vertexShader
    );

    glAttachShader(
        shaderProgram,
        fragmentShader
    );

    glLinkProgram(shaderProgram);


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // -----------------------------------------------------
    // VAO / VBO
    // -----------------------------------------------------

    unsigned int VAO;
    unsigned int VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

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


    // -----------------------------------------------------
    // POSITION ATTRIBUTE
    // -----------------------------------------------------

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);


    // -----------------------------------------------------
    // COLOR ATTRIBUTE
    // -----------------------------------------------------

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);


    // -----------------------------------------------------
    // MATRICES
    // -----------------------------------------------------

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;


    // Perspective
    projection = glm::perspective(
        glm::radians(45.0f),
        (float)SCR_WIDTH / (float)SCR_HEIGHT,
        0.1f,
        100.0f
    );


    // Camera / view
    view = glm::mat4(1.0f);

    view = glm::translate(
        view,
        glm::vec3(0.0f, -0.3f, -6.0f)
    );


    // -----------------------------------------------------
    // GET UNIFORM LOCATIONS
    // -----------------------------------------------------

    int modelLoc =
        glGetUniformLocation(
            shaderProgram,
            "model"
        );

    int viewLoc =
        glGetUniformLocation(
            shaderProgram,
            "view"
        );

    int projectionLoc =
        glGetUniformLocation(
            shaderProgram,
            "projection"
        );


    // -----------------------------------------------------
    // RENDER LOOP
    // -----------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        // Read keyboard
        Inputs(window);


        // -------------------------------------------------
        // CLEAR SCREEN
        // -------------------------------------------------

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


        // -------------------------------------------------
        // USE SHADER
        // -------------------------------------------------

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);


        // -------------------------------------------------
        // SEND VIEW AND PROJECTION
        // -------------------------------------------------

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


        // =================================================
        // TABLETOP
        // =================================================

        float tableWidth = 3.0f;
        float tableDepth = 2.0f;
        float tableHeight = 0.20f;

        float topY = 1.35f;

        model = glm::mat4(1.0f);

        model = glm::translate(
            model,
            glm::vec3(
                tableX,
                tableY + topY,
                tableZ
            )
        );

        model = glm::scale(
            model,
            glm::vec3(
                tableWidth,
                tableHeight,
                tableDepth
            )
        );

        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );


        // =================================================
        // LEGS
        // =================================================

        float legWidth = 0.25f;
        float legDepth = 0.25f;
        float legHeight = 2.50f;

        // Calculate corner positions automatically
        float legX =
            (tableWidth / 2.0f) -
            (legWidth / 2.0f);

        float legZ =
            (tableDepth / 2.0f) -
            (legDepth / 2.0f);

        // The center of the legs is slightly above
        // the ground so that they reach the tabletop.
        float legY = 0.0f;


        // -------------------------------------------------
        // LEG 1 - FRONT LEFT
        // -------------------------------------------------

        model = glm::mat4(1.0f);

        model = glm::translate(
            model,
            glm::vec3(
                tableX - legX,
                tableY + legY,
                tableZ + legZ
            )
        );

        model = glm::scale(
            model,
            glm::vec3(
                legWidth,
                legHeight,
                legDepth
            )
        );

        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );


        // -------------------------------------------------
        // LEG 2 - FRONT RIGHT
        // -------------------------------------------------

        model = glm::mat4(1.0f);

        model = glm::translate(
            model,
            glm::vec3(
                tableX + legX,
                tableY + legY,
                tableZ + legZ
            )
        );

        model = glm::scale(
            model,
            glm::vec3(
                legWidth,
                legHeight,
                legDepth
            )
        );

        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );


        // -------------------------------------------------
        // LEG 3 - BACK LEFT
        // -------------------------------------------------

        model = glm::mat4(1.0f);

        model = glm::translate(
            model,
            glm::vec3(
                tableX - legX,
                tableY + legY,
                tableZ - legZ
            )
        );

        model = glm::scale(
            model,
            glm::vec3(
                legWidth,
                legHeight,
                legDepth
            )
        );

        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );


        // -------------------------------------------------
        // LEG 4 - BACK RIGHT
        // -------------------------------------------------

        model = glm::mat4(1.0f);

        model = glm::translate(
            model,
            glm::vec3(
                tableX + legX,
                tableY + legY,
                tableZ - legZ
            )
        );

        model = glm::scale(
            model,
            glm::vec3(
                legWidth,
                legHeight,
                legDepth
            )
        );

        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );


        // -------------------------------------------------
        // SHOW FRAME
        // -------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // -----------------------------------------------------
    // CLEANUP
    // -----------------------------------------------------

    glDeleteVertexArrays(
        1,
        &VAO
    );

    glDeleteBuffers(
        1,
        &VBO
    );

    glDeleteProgram(
        shaderProgram
    );

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
