#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Window
{
	void Create(int width, int height);
	bool ShouldClose();
	void HandleInputEvents();
	void ClearScreenBuffer(float red, float green, float blue, float alpha);
	void SwapScreenBuffer();

	void MouseCallback(GLFWwindow* window, double xpos, double ypos);
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void GlfwErrorCallback(int errorCode, const char* description);
	void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
}
