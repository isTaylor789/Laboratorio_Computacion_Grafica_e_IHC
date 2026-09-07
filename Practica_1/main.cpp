#include <iostream>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Error: could not initialize GLFW\n";
        return 1;
    }

    // Request an OpenGL context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        800,
        600,
        "Practica 1 - Triangulo",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cerr << "Error: could not create GLFW window\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    // GPU / OpenGL information
    std::cout << "========================================\n";
    std::cout << "       INFORMACION DE GRAFICOS\n";
    std::cout << "========================================\n";

    std::cout << "GPU Vendor : "
              << glGetString(GL_VENDOR) << '\n';

    std::cout << "GPU Model  : "
              << glGetString(GL_RENDERER) << '\n';

    std::cout << "OpenGL     : "
              << glGetString(GL_VERSION) << '\n';

    std::cout << "GLSL       : "
              << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

    std::cout << "========================================\n";

    // Background color
    glClearColor(0.20f, 0.30f, 0.29f, 1.0f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // Black triangle
        glColor3f(0.0f, 0.0f, 0.0f);

        glBegin(GL_TRIANGLES);

            glVertex2f( 0.0f,  0.65f);
            glVertex2f(-0.50f, -0.45f);
            glVertex2f( 0.50f, -0.45f);

        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
