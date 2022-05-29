#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>

#pragma warning(pop)

#include "application.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "jUtil.hpp"
#include "text.hpp"
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

    unsigned int Indices[] = {
    0, 1, 2, // first triangle
    2, 3, 0  // second triangle
    };

    float Vertices[] = {
        // positions         // texture coords
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, // top left 
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f  // bottom left
    };

    unsigned int VertexArrayObject;
    unsigned int VertexBufferObject;
    unsigned int ElementBufferObject;

    glGenVertexArrays(1, &VertexArrayObject);
    glGenBuffers(1, &VertexBufferObject);
    glGenBuffers(1, &ElementBufferObject);

    glBindVertexArray(VertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Texture attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    JAssert(VertexArrayObject != 0, "VertexArrayObject isn't initialized, cannot init sprite");
    JAssert(VertexBufferObject != 0, "VertexBufferObject isn't initialized, cannot init sprite");
    JAssert(ElementBufferObject != 0, "ElementBufferObject isn't initialized, cannot init sprite");

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
        glBindVertexArray(VertexArrayObject);
        glDrawElements(GL_TRIANGLES, sizeof(Vertices), GL_UNSIGNED_INT, 0);

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
