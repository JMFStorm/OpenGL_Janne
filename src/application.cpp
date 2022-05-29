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
#include "text.h"
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
    std::map<char, Character>* characters = loadCharacters("fonts/arial.ttf");

    // Load texture1
    int texture1 = Texture::Create("./textures/container.jpg", false);

    // Load shader1
    Shader* shader1 = new Shader(
        "MyShader_01",
        "./shaders/default_vertex_shader.shader",
        "./shaders/default_fragment_shader.shader");

    shader1->SetInt("texture1", 0);

    while (!GetWindowShouldClose())
    {
        appState.deltaTime = CalculateDeltatime();

        HandleInputEvents();

        ClearWindowBuffer(0.2f, 0.3f, 0.3f, 1.0f);

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
