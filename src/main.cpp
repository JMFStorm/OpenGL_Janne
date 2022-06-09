#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

#include "main.hpp"
#include "freeType.hpp"
#include "image.hpp"
#include "jUtil.hpp"
#include "shader.hpp"
#include "vertexArray.hpp"
#include "window.hpp"

unsigned int CreateVertexBuffer(const std::vector<float> vertices)
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

unsigned int CreateIndexBuffer(const std::vector<unsigned int> indices)
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

unsigned int CreateTexture(const std::string& filePath, const bool isRGBA)
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

void BindTexture(unsigned int textureId)
{
    JAssert(textureId != 0, "Id missing, cannot bind texture ");
    glBindTexture(GL_TEXTURE_2D, textureId);
}

struct FpsCounter
{
    unsigned long frames;
    unsigned int displayFps;
    float previousCurrentTime;
    float currentTime;
    float lastFpsCalcTime;
    float deltaTime;
    float overflowedFpsCalcTime;
};

struct ApplicationState
{
    FpsCounter fpsCounter;
};

void IncrementFrames(FpsCounter* fpsCounter, const unsigned int frames)
{
    fpsCounter->frames += frames;

}
void CalculateDeltatime(FpsCounter* fpsCounter)
{
    fpsCounter->currentTime = (float)glfwGetTime();
    fpsCounter->deltaTime = fpsCounter->currentTime - fpsCounter->lastFpsCalcTime;
    fpsCounter->lastFpsCalcTime = fpsCounter->currentTime;
}

void CalculateFpsScuffed(FpsCounter* fpsCounter)
{
    const float second = 1.0f;
    const float secondsElapsedFromPrevious = 
        (fpsCounter->currentTime + fpsCounter->overflowedFpsCalcTime)
        - fpsCounter->previousCurrentTime;

    if (second < secondsElapsedFromPrevious)
    {
        fpsCounter->displayFps = fpsCounter->frames;
        fpsCounter->frames = 0;
        fpsCounter->previousCurrentTime = fpsCounter->currentTime;
        fpsCounter->overflowedFpsCalcTime = secondsElapsedFromPrevious - second;
    }
}

int RunApplication()
{
    FreeTypeFont gDebugFTFont = {};
    ApplicationState appState = {};

    int result;

    result = glfwInit();
    JAssert(result == GLFW_TRUE, "glfwInit() failed");

    GLFWwindow* window = CreateWindow(false);

    // Load GLAD context
    result = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    JAssert(result == 1, "Failed to initialize OpenGL context GLAD");

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &result);

    printf("Using OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
    printf("Maximum nr of vertex attributes supported: %d\n", result);

    // Load text fonts for debug
    LoadFreeTypeFont("fonts/Roboto-Regular.ttf", &gDebugFTFont);

    // Load texture1
    unsigned int texture1 = CreateTexture("./images/awesomeface.png", true);

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

    unsigned int vertexArrayObject = CreateVertexArray();

    BindVertexArray(vertexArrayObject);

    CreateVertexBuffer(vertices);
    CreateIndexBuffer(indices);

    const int vertexAttributeStride = 5 * sizeof(float);
    const int secondAttribOffset = 3;

    SetVertexAttributePointer(
        0,
        3,
        ShaderDataType::Float,
        false,
        vertexAttributeStride,
        (void*)0);

    SetVertexAttributePointer(
        1,
        2,
        ShaderDataType::Float,
        false,
        vertexAttributeStride,
        (void*)(secondAttribOffset * sizeof(float)));

    UnbindVertexArray();

    unsigned int shader1 = CreateShader(
        "./shaders/default_vertex_shader.shader",
        "./shaders/default_fragment_shader.shader");

    // glm::mat4 trans = glm::mat4(1.0f);

    UseShader(shader1);
    // Shader::SetMat4(shader1, "transform", trans);
    SetInt(shader1, "texture1", 0);
    UseShader(0);


    while (!WindowShouldClose(window))
    {
        HandleWindowInputEvents(window);
        ClearScreenBuffer(0.2f, 0.3f, 0.3f, 1.0f);

        BindTexture(texture1);
        UseShader(shader1);

        // Translations
        glm::mat4 trans = glm::mat4(1.0f);
        double time = glfwGetTime();
        float rotateScale = (float)time * 40;
        trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f));
        trans = glm::rotate(trans, glm::radians(rotateScale * -4), glm::vec3(0.0f, 0.0f, 1.0f));
        trans = glm::scale(trans, glm::vec3(1.0f, 1.0f, 1.0f));
        SetMat4(shader1, "transform", trans);
        //

        glActiveTexture(GL_TEXTURE0);
        BindVertexArray(vertexArrayObject);

        int elementsCount = (int)indices.size();
        glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, 0);
        UseShader(0);

        std::stringstream stream;
        stream << std::fixed << std::setprecision(3) << rotateScale;
        auto displayDebug1 = "rotateScale: " + stream.str();

        stream.str("");
        stream << std::fixed << std::setprecision(1) << appState.fpsCounter.currentTime;
        auto displayCurrent = "Time: " + stream.str() + "s";

        stream.str("");
        stream << std::fixed << std::setprecision(1) << appState.fpsCounter.displayFps;
        auto displayFps = "FPS: " + stream.str();

        // Benchmark
        auto start = std::chrono::high_resolution_clock::now();
        //

        RenderFreeTypeText(
            &gDebugFTFont,
            displayDebug1,
            30.0f,
            1160.0f,
            1.0f,
            glm::vec3(0.8, 0.8f, 0.8f));

        RenderFreeTypeText(
            &gDebugFTFont,
            displayFps,
            1350.0f,
            1160.0f,
            1.0f,
            glm::vec3(0.8, 0.8f, 0.8f));

        RenderFreeTypeText(
            &gDebugFTFont,
            displayCurrent,
            1350.0f,
            1130.0f,
            1.0f,
            glm::vec3(0.8, 0.8f, 0.8f));

        // Benchmark
        auto end = std::chrono::high_resolution_clock::now();
        auto startRes = std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch();
        auto endRes = std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch();
        auto duration = (endRes - startRes);
        std::cout << "Text render microseconds: " << duration.count() << "\n";
        //

        SwapScreenBuffer(window);

        IncrementFrames(&appState.fpsCounter, 1);
        CalculateDeltatime(&appState.fpsCounter);
        CalculateFpsScuffed(&appState.fpsCounter);
    }

    return 0;
}


int main()
{
    int code = 0;
    std::setlocale(LC_ALL, "utf-8");

	code = RunApplication();
	return code;
}
