#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow* GetWindow(int width, int height);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void glfwErrorCallback(int errorCode, const char* description);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
