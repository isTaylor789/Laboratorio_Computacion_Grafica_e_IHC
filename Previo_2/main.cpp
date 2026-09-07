#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// ============================================================
// 1. VERTEX SHADER
// ============================================================
//
// The vertex shader receives each vertex individually.
//
// Each vertex has:
//   - Position: X, Y
//   - Color:    R, G, B
//
// The position is sent to the graphics pipeline.
// The color is passed to the fragment shader.
//
// ============================================================

const char* vertexShaderSource = R"(
#version 330 core

// Position of the current vertex
layout (location = 0) in vec2 aPos;

// Color of the current vertex
layout (location = 1) in vec3 aColor;

// This value will be sent to the fragment shader.
// Because it is an "out" variable, OpenGL will interpolate
// the color between the vertices.
out vec3 vertexColor;

void main()
{
    // The vertex position is converted to a 4-component
    // coordinate because OpenGL works with homogeneous coordinates.
    gl_Position = vec4(aPos, 0.0, 1.0);

    // Send the color to the next stage of the pipeline.
    vertexColor = aColor;
}
)";

// ============================================================
// 2. FRAGMENT SHADER
// ============================================================
//
// The fragment shader determines the final color of each
// pixel that belongs to the triangle.
//
// "vertexColor" is automatically interpolated between the
// three vertices, which creates the gradient.
//
// ============================================================

const char* fragmentShaderSource = R"(
#version 330 core

// Color received from the vertex shader
in vec3 vertexColor;

// Final color of the pixel
out vec4 FragColor;

void main()
{
    // The RGB color comes from the interpolated vertex color.
    // Alpha = 1.0 means completely opaque.
    FragColor = vec4(vertexColor, 1.0);
}
)";

// ============================================================
// FUNCTION: checkShaderCompilation
// ============================================================
//
// Checks whether a shader was compiled correctly.
//
// ============================================================

void checkShaderCompilation(GLuint shader, const std::string& name)
{
    GLint success;
    char infoLog[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);

        std::cerr << "Shader compilation error (" << name << "):\n"
                  << infoLog << std::endl;
    }
}

// ============================================================
// FUNCTION: checkProgramLinking
// ============================================================
//
// A shader program is the combination of the vertex shader
// and fragment shader.
//
// Here we check that both were successfully linked together.
//
// ============================================================

void checkProgramLinking(GLuint program)
{
    GLint success;
    char infoLog[512];

    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);

        std::cerr << "Program linking error:\n"
                  << infoLog << std::endl;
    }
}

// ============================================================
// MAIN
// ============================================================

int main()
{
    // ========================================================
    // 3. INITIALIZE GLFW
    // ========================================================
    //
    // GLFW creates the window and OpenGL context.
    //
    // A context is the environment where OpenGL stores
    // the state necessary to render graphics.
    //
    // ========================================================

    if (!glfwInit())
    {
        std::cerr << "Could not initialize GLFW." << std::endl;
        return -1;
    }

    // We request OpenGL 3.3 Core Profile.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ========================================================
    // 4. CREATE WINDOW
    // ========================================================

    GLFWwindow* window = glfwCreateWindow(
        800,
        600,
        "2D Triangle - OpenGL",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cerr << "Could not create the window." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // ========================================================
    // 5. INITIALIZE GLEW
    // ========================================================
    //
    // GLEW allows us to access modern OpenGL functions.
    //
    // ========================================================

    glewExperimental = GL_TRUE;

    GLenum glewStatus = glewInit();

    if (glewStatus != GLEW_OK &&
        glewStatus != GLEW_ERROR_NO_GLX_DISPLAY)
    {
        std::cerr << "GLEW initialization failed: "
                  << glewGetErrorString(glewStatus)
                  << std::endl;

        glfwTerminate();
        return -1;
    }

    // ========================================================
    // 6. DEFINE THE VERTICES
    // ========================================================
    //
    // A vertex is a point in our figure.
    //
    // Each vertex contains:
    //
    //     X, Y  -> position
    //     R, G, B -> color
    //
    // We create 3 vertices.
    //
    // Vertex 0 = upper point
    // Vertex 1 = lower-left point
    // Vertex 2 = lower-right point
    //
    // These 3 vertices will be connected by OpenGL
    // to create 3 edges.
    //
    // The 3 edges together form one 2D triangle.
    //
    // ========================================================

    float vertices[] =
    {
        // Position       // Color
         0.0f,  0.7f,     0.0f, 1.0f, 1.0f,   // Vertex 0: turquoise

        -0.7f, -0.6f,     0.0f, 1.0f, 0.6f,   // Vertex 1: green

         0.7f, -0.6f,     0.2f, 1.0f, 0.8f    // Vertex 2: aqua
    };

    // ========================================================
    // 7. CREATE VAO AND VBO
    // ========================================================
    //
    // VBO = Vertex Buffer Object
    // It stores the vertex information in GPU memory.
    //
    // VAO = Vertex Array Object
    // It remembers how OpenGL should interpret that data.
    //
    // ========================================================

    GLuint VAO;
    GLuint VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Activate the VAO
    glBindVertexArray(VAO);

    // Activate the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copy the vertices from CPU memory to GPU memory
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    // ========================================================
    // 8. TELL OPENGL HOW THE VERTEX DATA IS ORGANIZED
    // ========================================================
    //
    // Each vertex contains 5 float values:
    //
    // [ X, Y, R, G, B ]
    //
    // Attribute 0 -> position
    // Attribute 1 -> color
    //
    // ========================================================

    // ---- POSITION ----
    glVertexAttribPointer(
        0,                  // Attribute location
        2,                  // Number of values: X, Y
        GL_FLOAT,           // Data type
        GL_FALSE,
        5 * sizeof(float),  // Distance from one vertex to the next
        (void*)0            // Position starts at the beginning
    );

    glEnableVertexAttribArray(0);

    // ---- COLOR ----
    glVertexAttribPointer(
        1,                          // Attribute location
        3,                          // Number of values: R, G, B
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),          // Distance between vertices
        (void*)(2 * sizeof(float))  // Color starts after X,Y
    );

    glEnableVertexAttribArray(1);

    // ========================================================
    // 9. CREATE THE VERTEX SHADER
    // ========================================================

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        nullptr
    );

    glCompileShader(vertexShader);

    checkShaderCompilation(vertexShader, "Vertex Shader");

    // ========================================================
    // 10. CREATE THE FRAGMENT SHADER
    // ========================================================

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        nullptr
    );

    glCompileShader(fragmentShader);

    checkShaderCompilation(fragmentShader, "Fragment Shader");

    // ========================================================
    // 11. CREATE SHADER PROGRAM
    // ========================================================
    //
    // Now we connect both shaders.
    //
    // Vertex Shader
    //       |
    //       v
    // Fragment Shader
    //
    // Together they form the program used for rendering.
    //
    // ========================================================

    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    checkProgramLinking(shaderProgram);

    // The individual shaders are no longer necessary
    // after they have been linked into the program.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ========================================================
    // 12. RENDER LOOP
    // ========================================================
    //
    // The program repeatedly:
    //
    // 1. Clears the screen
    // 2. Activates the shader program
    // 3. Activates the VAO
    // 4. Draws the 3 vertices as a triangle
    // 5. Displays the result
    //
    // ========================================================

    while (!glfwWindowShouldClose(window))
    {
        // Clear the screen with black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Activate our shaders
        glUseProgram(shaderProgram);

        // Activate the vertex configuration
        glBindVertexArray(VAO);

        // ====================================================
        // DRAW THE FIGURE
        // ====================================================
        //
        // GL_TRIANGLES tells OpenGL:
        //
        // "Take every group of 3 vertices and form a triangle."
        //
        // We have exactly 3 vertices, so OpenGL creates:
        //
        //          Vertex 0
        //             /\
        //            /  \
        //           /    \
        //     Vertex 1----Vertex 2
        //
        // The 3 sides are the edges of the triangle.
        //
        // ====================================================

        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Show the rendered image
        glfwSwapBuffers(window);

        // Process keyboard/window events
        glfwPollEvents();
    }

    // ========================================================
    // 13. CLEANUP
    // ========================================================

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
