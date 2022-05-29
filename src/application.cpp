#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <vector>

#pragma warning(pop)

#include "application.hpp"
#include "buffer.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "jUtil.hpp"
#include "text.hpp"
#include "vertexArray.hpp"
#include "window.hpp"

struct ApplicationState
{
    float currentFrameTime;
    float deltaTime;
    float lastFrame;
} appState;

int RunApplication()
{
    appState.currentFrameTime = 0.0f;
    appState.deltaTime = 0.0f;
    appState.lastFrame = 0.0f;

    CreateWindow(1800, 1200);

    // Load text fonts
    std::map<char, Character>* characters = LoadCharacters("fonts/arial.ttf");

    // Load texture1
    unsigned int texture1 = Texture::Create("./images/container.jpg", false);

    std::vector<unsigned int> indices{
        0, 1, 2, // first triangle
        2, 3, 0  // second triangle
    };

    std::vector<float> vertices {
        // positions         // texture coords
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, // top left 
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f  // bottom left
    };

    unsigned int VertexArrayObject = VertexArray::Create();
    VertexArray::Bind(VertexArrayObject);

    unsigned int VertexBufferObject = VertexBuffer::Create(vertices);
    unsigned int ElementBufferObject = IndexBuffer::Create(indices);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Texture attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int shader1 = Shader::Create(
        "./shaders/default_vertex_shader.shader",
        "./shaders/default_fragment_shader.shader");

    Shader::SetInt(shader1, "texture1", 0);

    while (!GetWindowShouldClose())
    {
        appState.deltaTime = CalculateDeltatime();

        HandleInputEvents();

        ClearWindowBuffer(0.2f, 0.3f, 0.3f, 1.0f);

        Texture::Bind(texture1);
        Shader::Use(shader1);

        glActiveTexture(GL_TEXTURE0);

        VertexArray::Bind(VertexArrayObject);

        int elementsCount = indices.size();
        glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, 0);

        SwapScreenBuffer();
    }

    return 0;
}

float CalculateDeltatime()
{
    appState.currentFrameTime = glfwGetTime();
    appState.lastFrame = appState.currentFrameTime;

    float deltatime = appState.currentFrameTime - appState.lastFrame;
    return deltatime;
}
