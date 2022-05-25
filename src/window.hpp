#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void CreateWindow(int width, int height);
bool GetWindowShouldClose();
void HandleInputEvents();
void ClearWindowBuffer(float red, float green, float blue, float alpha);
void SwapScreenBuffer();

void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void glfwErrorCallback(int errorCode, const char* description);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
