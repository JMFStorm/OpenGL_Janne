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

    Window::Create(1800, 1200);

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

    unsigned int vertexArrayObject = VertexArray::Create();

    VertexArray::Bind(vertexArrayObject);

    unsigned int VertexBufferObject = VertexBuffer::Create(vertices);
    unsigned int ElementBufferObject = IndexBuffer::Create(indices);

    int vertexAttributeStride = 5 * sizeof(float);
    int secondAttribOffset = 3;

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

    Shader::SetInt(shader1, "texture1", 0);

    while (!Window::ShouldClose())
    {
        appState.deltaTime = CalculateDeltatime();

        Window::HandleInputEvents();

        Window::ClearScreenBuffer(0.2f, 0.3f, 0.3f, 1.0f);

        Texture::Bind(texture1);
        Shader::Use(shader1);

        glActiveTexture(GL_TEXTURE0);

        VertexArray::Bind(vertexArrayObject);

        int elementsCount = indices.size();
        glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, 0);

        Window::SwapScreenBuffer();
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
