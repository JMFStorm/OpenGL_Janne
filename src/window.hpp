#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void ClearScreenBuffer(const float red, const float green, const float blue, const float alpha);

void SwapScreenBuffer(GLFWwindow* window);

void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void GlfwErrorCallback(int errorCode, const char* description);

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

GLFWwindow* CreateWindow(const bool fullscreen);

bool WindowShouldClose(GLFWwindow* window);

void HandleWindowInputEvents(GLFWwindow* window);