#include <iostream>
#include <stdio.h>

#include "window.hpp"
#include "jUtil.hpp"

GLFWwindow* window;

void CreateWindow(int width, int height)
{
    JAssert(glfwInit() == GLFW_TRUE, "glfwInit() failed");

    window = glfwCreateWindow(width, height, "LearnOpenGL!", NULL, NULL);

    JAssert(window != NULL, "glfwCreateWindow() failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback(glfwErrorCallback);

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load GLAD context
    int loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    JAssert(loaded == 1, "Failed to initialize OpenGL context GLAD");

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);

    printf("Using OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
    printf("Maximum nr of vertex attributes supported: %d\n", nrAttributes);
}

bool GetWindowShouldClose()
{
    return glfwWindowShouldClose(window);
}

void HandleInputEvents()
{
    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        // cameraTransform = glm::translate(cameraTransform, glm::vec3(0.0f, -1.0f, 0.0f) * cameraSpeed);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        // cameraTransform = glm::translate(cameraTransform, glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        // cameraTransform = glm::translate(cameraTransform, glm::vec3(1.0f, 0.0f, 0.0f) * cameraSpeed);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        // cameraTransform = glm::translate(cameraTransform, glm::vec3(-1.0f, 0.0f, 0.0f) * cameraSpeed);
    }
}

void ClearWindowBuffer(float red, float green, float blue, float alpha)
{
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SwapScreenBuffer()
{
    glfwSwapBuffers(window);
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
