#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdio.h>
#include <thread>
#include <vector>

#pragma warning(pop)

#include "freeType.hpp"
#include "jUtil.hpp"
#include "shader.hpp"
#include "vertexArray.hpp"

#define WINDOW_WIDTH_DEFAULT 1600
#define WINDOW_HEIGHT_DEFAULT 1200

unsigned char* LoadImage(const char* filePath, int* width, int* height, int* nrChannels)
{
    stbi_set_flip_vertically_on_load(true);
    return stbi_load(filePath, width, height, nrChannels, 0);
}

void FreeImageData(unsigned char* data)
{
    stbi_image_free(data);
}

namespace VertexBuffer
{
    unsigned int Create(const std::vector<float> vertices)
    {
        unsigned int vertexBufferObject;

        glGenBuffers(1, &vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

        int sizeInBytes = (int)vertices.size() * sizeof(unsigned int);

        glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &vertices[0], GL_STATIC_DRAW);

        GLint size = 0;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

        if (sizeInBytes != size)
        {
            glDeleteBuffers(1, &vertexBufferObject);
            JAssert(sizeInBytes != size, "VertexBufferObject GL_BUFFER_SIZE failed");
        }

        return vertexBufferObject;
    }
}

namespace IndexBuffer
{
    unsigned int Create(const std::vector<unsigned int> indices)
    {
        unsigned int indexBufferObject;

        glGenBuffers(1, &indexBufferObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);

        int sizeInBytes = (int)indices.size() * sizeof(unsigned int);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeInBytes, &indices[0], GL_STATIC_DRAW);

        GLint size = 0;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

        if (sizeInBytes != size)
        {
            glDeleteBuffers(1, &indexBufferObject);
            JAssert(sizeInBytes != size, "IndexBufferObject GL_BUFFER_SIZE failed");
        }

        return indexBufferObject;
    }
}

namespace Texture
{
    unsigned int Create(const std::string& filePath, const bool isRGBA)
    {
        JAssert(filePath.empty() == false, "FilePath missing, cannot intit texture ");

        unsigned int textureId;
        int rbgMode = isRGBA ? GL_RGBA : GL_RGB;

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;

        unsigned char* data = LoadImage(filePath.c_str(), &width, &height, &nrChannels);

        JAssert(data, "Failed to load texture: " + filePath);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, rbgMode, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        FreeImageData(data);

        return textureId;
    }

    void Bind(unsigned int textureId)
    {
        JAssert(textureId != 0, "Id missing, cannot bind texture ");
        glBindTexture(GL_TEXTURE_2D, textureId);
    }
}

namespace Window
{
    void ClearScreenBuffer(const float red, const float green, const float blue, const float alpha)
    {
        glClearColor(red, green, blue, alpha);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SwapScreenBuffer(GLFWwindow* window)
    {
        glfwSwapBuffers(window);
    }

    void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
    {
    }

    void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
    }

    void GlfwErrorCallback(int errorCode, const char* description)
    {
        printf("GLFW error %d: %s\n", errorCode, description);
    }

    void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    GLFWwindow* Create(const bool fullscreen)
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        GLFWwindow* window;
        auto fullscreenOption = fullscreen ? monitor : NULL;
        auto widthOption = fullscreen ? mode->width : WINDOW_WIDTH_DEFAULT;
        auto heightOption = fullscreen ? mode->height : WINDOW_HEIGHT_DEFAULT;

        window = glfwCreateWindow(widthOption, heightOption, "LearnOpenGL!", fullscreenOption, NULL);
        JAssert(window != NULL, "glfwCreateWindow() failed");

        glfwMakeContextCurrent(window);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwSetErrorCallback(GlfwErrorCallback);
        glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
        glfwSetCursorPosCallback(window, MouseCallback);
        glfwSetScrollCallback(window, ScrollCallback);

        // tell GLFW to capture our mouse
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        return window;
    }

    bool ShouldClose(GLFWwindow* window)
    {
        return glfwWindowShouldClose(window);
    }

    void HandleInputEvents(GLFWwindow* window)
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
}

struct FpsCounter
{
    unsigned long frames;
    unsigned int displayFps;
    double previousCurrentTime;
    double currentTime;
    double lastFpsCalcTime;
    double deltaTime;
    double overflowedFpsCalcTime;
};

struct ApplicationState
{
    FpsCounter fpsCounter;
};

namespace Application
{
    void IncrementFrames(FpsCounter* fpsCounter, const unsigned int frames)
    {
        fpsCounter->frames += frames;

    }
    void CalculateDeltatime(FpsCounter* fpsCounter)
    {
        fpsCounter->currentTime = glfwGetTime();
        fpsCounter->deltaTime = fpsCounter->currentTime - fpsCounter->lastFpsCalcTime;
        fpsCounter->lastFpsCalcTime = fpsCounter->currentTime;
    }

    void CalculateFpsScuffed(FpsCounter* fpsCounter)
    {
        const double second = 1.0f;
        const double secondsElapsedFromPrevious = (fpsCounter->currentTime + fpsCounter->overflowedFpsCalcTime)
            - fpsCounter->previousCurrentTime;

        if (second < secondsElapsedFromPrevious)
        {
            fpsCounter->displayFps = fpsCounter->frames;
            fpsCounter->frames = 0;
            fpsCounter->previousCurrentTime = fpsCounter->currentTime;
            fpsCounter->overflowedFpsCalcTime = secondsElapsedFromPrevious - second;
        }
    }

    int Run()
    {
        FreeType::Font gDebugFTFont = {};
        ApplicationState appState = {};

        int result;

        result = glfwInit();
        JAssert(result == GLFW_TRUE, "glfwInit() failed");

        GLFWwindow* window = Window::Create(false);

        // Load GLAD context
        result = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        JAssert(result == 1, "Failed to initialize OpenGL context GLAD");

        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &result);

        printf("Using OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
        printf("Maximum nr of vertex attributes supported: %d\n", result);

        // Load text fonts for debug
        FreeType::LoadFont("fonts/Roboto-Regular.ttf", &gDebugFTFont);

        // Load texture1
        unsigned int texture1 = Texture::Create("./images/awesomeface.png", true);

        std::vector<unsigned int> indices{
            0, 1, 2, // first triangle
            2, 3, 0  // second triangle
        };

        std::vector<float> vertices{
            // positions         // texture coords
            -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, // top left 
             0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right
             0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f  // bottom left
        };

        unsigned int vertexArrayObject = VertexArray::Create();

        VertexArray::Bind(vertexArrayObject);

        unsigned int VertexBufferObject = VertexBuffer::Create(vertices);
        unsigned int ElementBufferObject = IndexBuffer::Create(indices);

        const int vertexAttributeStride = 5 * sizeof(float);
        const int secondAttribOffset = 3;

        VertexArray::SetVertexAttributePointer(
            0,
            3,
            ShaderDataType::Float,
            false,
            vertexAttributeStride,
            (void*)0);

        VertexArray::SetVertexAttributePointer(
            1,
            2,
            ShaderDataType::Float,
            false,
            vertexAttributeStride,
            (void*)(secondAttribOffset * sizeof(float)));

        VertexArray::Unbind();

        unsigned int shader1 = Shader::Create(
            "./shaders/default_vertex_shader.shader",
            "./shaders/default_fragment_shader.shader");

        glm::mat4 trans = glm::mat4(1.0f);

        Shader::Use(shader1);
        Shader::SetMat4(shader1, "transform", trans);
        Shader::SetInt(shader1, "texture1", 0);
        Shader::Use(0);


        while (!Window::ShouldClose(window))
        {
            Window::HandleInputEvents(window);
            Window::ClearScreenBuffer(0.2f, 0.3f, 0.3f, 1.0f);

            Texture::Bind(texture1);
            Shader::Use(shader1);

            //
            float scaleAmount = static_cast<float>(sin(glfwGetTime()));
            trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f));
            trans = glm::rotate(trans, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            trans = glm::scale(trans, glm::vec3(1.0f, 1.0f, 1.0f));
            Shader::SetMat4(shader1, "transform", trans);
            //

            glActiveTexture(GL_TEXTURE0);
            VertexArray::Bind(vertexArrayObject);

            int elementsCount = (int)indices.size();
            glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, 0);
            Shader::Use(0);

            std::stringstream stream;
            stream << std::fixed << std::setprecision(2) << scaleAmount;
            auto displayDebug1 = "Scale amount: : " + stream.str();

            stream.str("");
            stream << std::fixed << std::setprecision(1) << appState.fpsCounter.currentTime;
            auto displayCurrent = "Time: " + stream.str() + "s";

            stream.str("");
            stream << std::fixed << std::setprecision(1) << appState.fpsCounter.displayFps;
            auto displayFps = "FPS: " + stream.str();

            FreeType::RenderText(
                &gDebugFTFont,
                displayDebug1,
                30.0f,
                1160.0f,
                1.0f,
                glm::vec3(0.8, 0.8f, 0.8f));

            FreeType::RenderText(
                &gDebugFTFont,
                displayFps,
                1350.0f,
                1160.0f,
                1.0f,
                glm::vec3(0.8, 0.8f, 0.8f));

            FreeType::RenderText(
                &gDebugFTFont,
                displayCurrent,
                1350.0f,
                1130.0f,
                1.0f,
                glm::vec3(0.8, 0.8f, 0.8f));

            Window::SwapScreenBuffer(window);

            IncrementFrames(&appState.fpsCounter, 1);
            CalculateDeltatime(&appState.fpsCounter);
            CalculateFpsScuffed(&appState.fpsCounter);
        }

        return 0;
    }
}

int main()
{
    int code = 0;
    std::setlocale(LC_ALL, "utf-8");

	code = Application::Run();
	return code;
}
