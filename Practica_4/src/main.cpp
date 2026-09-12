#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static std::string readFile(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::cerr << "Could not open: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static GLuint compileShader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);

    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char info[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, info);
        std::cerr << "Shader compilation error:\n" << info << std::endl;
    }

    return shader;
}

static GLuint createProgram(
    const std::string& vertexPath,
    const std::string& fragmentPath)
{
    std::string vertexSource = readFile(vertexPath);
    std::string fragmentSource = readFile(fragmentPath);

    GLuint vertexShader =
        compileShader(GL_VERTEX_SHADER, vertexSource);

    GLuint fragmentShader =
        compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char info[1024];
        glGetProgramInfoLog(program, 1024, nullptr, info);
        std::cerr << "Program linking error:\n" << info << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

struct Part
{
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    glm::vec3 color;
};

static void drawBox(
    GLuint vao,
    GLuint program,
    const Part& part)
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, part.position);

    model = glm::rotate(
        model,
        glm::radians(part.rotation.x),
        glm::vec3(1, 0, 0)
    );

    model = glm::rotate(
        model,
        glm::radians(part.rotation.y),
        glm::vec3(0, 1, 0)
    );

    model = glm::rotate(
        model,
        glm::radians(part.rotation.z),
        glm::vec3(0, 0, 1)
    );

    model = glm::scale(model, part.scale);

    glUniformMatrix4fv(
        glGetUniformLocation(program, "model"),
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );

    glUniform3fv(
        glGetUniformLocation(program, "partColor"),
        1,
        glm::value_ptr(part.color)
    );

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =
        glfwCreateWindow(800, 800, "Box Octopus", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Window creation failed." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(
            reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "GLAD initialization failed." << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // One box: position + normal-independent face colors.
    float vertices[] =
    {
        // Front
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        // Back
         0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,

        // Left
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // Right
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,

         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,

        // Top
        -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,

         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,

        // Bottom
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,

         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f
    };

    GLuint VAO, VBO;

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

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    GLuint program = createProgram(
        SHADER_DIR "/box.vert",
        SHADER_DIR "/box.frag"
    );

    glm::vec3 cameraPos(3.8f, 3.2f, 6.0f);

    glm::mat4 view = glm::lookAt(
        cameraPos,
        glm::vec3(0.0f, 1.2f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        800.0f / 800.0f,
        0.1f,
        100.0f
    );

    glUseProgram(program);

    glUniformMatrix4fv(
        glGetUniformLocation(program, "view"),
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(program, "projection"),
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );

    std::vector<Part> creature;

    // Main head
    creature.push_back({
        {0.0f, 2.6f, 0.0f},
        {1.65f, 1.9f, 1.45f},
        {0, 0, 0},
        {0.05f, 0.38f, 0.39f}
    });

    // Lower body
    creature.push_back({
        {0.0f, 1.45f, 0.0f},
        {1.15f, 1.1f, 1.0f},
        {0, 0, 0},
        {0.035f, 0.27f, 0.29f}
    });

    // Four hanging tentacles
    creature.push_back({
        {-0.62f, 0.15f,  0.38f},
        {0.38f, 1.65f, 0.42f},
        {3, 0, -3},
        {0.03f, 0.30f, 0.31f}
    });

    creature.push_back({
        {0.62f, 0.15f,  0.38f},
        {0.38f, 1.65f, 0.42f},
        {-2, 0, 3},
        {0.025f, 0.23f, 0.25f}
    });

    creature.push_back({
        {-0.60f, 0.10f, -0.38f},
        {0.38f, 1.55f, 0.40f},
        {-3, 0, -3},
        {0.02f, 0.25f, 0.27f}
    });

    creature.push_back({
        {0.60f, 0.10f, -0.38f},
        {0.38f, 1.55f, 0.40f},
        {2, 0, 3},
        {0.02f, 0.28f, 0.29f}
    });

    // Side details
    creature.push_back({
        {-0.93f, 2.25f, 0.05f},
        {0.28f, 0.45f, 0.45f},
        {0, 0, 0},
        {0.08f, 0.50f, 0.49f}
    });

    creature.push_back({
        {0.93f, 2.45f, 0.0f},
        {0.22f, 0.55f, 0.42f},
        {0, 0, 0},
        {0.04f, 0.34f, 0.36f}
    });

    // Bright cyan blocks
    creature.push_back({
        {-0.78f, 2.75f, 0.72f},
        {0.22f, 0.38f, 0.18f},
        {0, 0, 0},
        {0.22f, 0.75f, 0.70f}
    });

    creature.push_back({
        {0.72f, 2.15f, 0.73f},
        {0.18f, 0.30f, 0.18f},
        {0, 0, 0},
        {0.18f, 0.62f, 0.61f}
    });

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        for (const Part& part : creature)
        {
            drawBox(VAO, program, part);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(program);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
