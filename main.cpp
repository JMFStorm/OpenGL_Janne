#pragma warning(push, 0)

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#pragma warning(pop)

// Function declarations

void glfwErrorCallback(int errorCode, const char* description);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

unsigned int createShaderProgram(const char* vertexFilePath, const char* fragmentFilePath);

std::string readFileToString(const char* filePath);

// Main program start

int main()
{
    if (glfwInit() == GLFW_FALSE)
    {
        printf("glfwInit() failed\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback(glfwErrorCallback);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "LearnOpenGL_2D!", NULL, NULL);

    if (window == NULL)
    {
        printf("glfwCreateWindow() failed\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Load all OpenGL functions using the glfw loader function
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize OpenGL context GLAD\n");
        return -1;
    }

    printf("Using OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    printf("Maximum nr of vertex attributes supported: %d\n",  nrAttributes);

    unsigned int shaderProgram = createShaderProgram("./vertex_shader.shader", "./fragment_shader.shader");
    
    float vertices[] = {
        // Positions         // Colors
         0.5f, -0.5f, 0.0f,  0.0f, 0.5f, 1.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.5f, 1.0f, 0.75f,  // bottom left
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.5f,   // top right
        -0.5f,  0.5f, 0.0f,  0.25f, 0.5f, 1.0f   // top left
    };

    unsigned int indices[] = {  
        3, 1, 2,  
        0, 1, 2   
    };

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    // Polor attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));


    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        float redValue = (sin(timeValue * 0.5f) / 2.0f) + 0.5f;
        float blueValue = (sin(timeValue * 2.0f) / 2.0f) + 0.5f;

        int vertexColorLocation = glGetUniformLocation(shaderProgram, "changingColor");
        glUniform4f(vertexColorLocation, redValue, greenValue, blueValue, 1.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

	return 0;
}

void glfwErrorCallback(int errorCode, const char* description)
{
    printf("GLFW error %d: %s\n", errorCode, description);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

std::string readFileToString(const char* filePath)
{
    std::string shaderCode;
    std::ifstream shaderFile;
    std::stringstream shaderStream;

    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        shaderFile.open(filePath);
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();

        shaderCode = shaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        printf("File read failed: %s\n", e.what());
    }

    return shaderCode;
}

unsigned int createShaderProgram(const char* vertexFilePath, const char* fragmentFilePath)
{
    int success;
    char infoLog[512];

    // Get shader source strings

    std::string vertexSourceString = readFileToString(vertexFilePath);
    std::string fragmentSourceString = readFileToString(fragmentFilePath);

    const char* vertexSource = vertexSourceString.c_str();
    const char* fragmentSource = fragmentSourceString.c_str();

    // Create vertex shader

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex shader compilation failed: %s\n", infoLog);
    }

    // Create fragment shader

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment shader compilation failed: %s\n", infoLog);
    }

    // Create new shader program

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Program linking failed: %s\n", infoLog);
    }

    // Cleanup

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}