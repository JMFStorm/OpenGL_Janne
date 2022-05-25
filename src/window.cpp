#include <iostream>
#include <stdio.h>

#include "window.hpp"
#include "jUtil.hpp"

Window::Window(int width, int height)
{
    WindowInstance = glfwCreateWindow(width, height, "LearnOpenGL!", NULL, NULL);

    jAssert(WindowInstance != NULL, "glfwCreateWindow() failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback(glfwErrorCallback);

    glfwMakeContextCurrent(WindowInstance);
    glfwSetFramebufferSizeCallback(WindowInstance, framebufferSizeCallback);
    glfwSetCursorPosCallback(WindowInstance, mouseCallback);
    glfwSetScrollCallback(WindowInstance, scrollCallback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load GLAD context
    // Load all OpenGL functions using the glfw loader function
    int loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    jAssert(loaded == 1, "Failed to initialize OpenGL context GLAD");

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);

    printf("Using OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
    printf("Maximum nr of vertex attributes supported: %d\n", nrAttributes);
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
}

void glfwErrorCallback(int errorCode, const char* description)
{
    printf("GLFW error %d: %s\n", errorCode, description);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
