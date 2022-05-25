#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	GLFWwindow* WindowInstance;

	Window(int width, int height);

private:

};

void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void glfwErrorCallback(int errorCode, const char* description);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
