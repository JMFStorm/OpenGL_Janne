#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>

#pragma warning(pop)

#include "application.hpp"
#include "sprite.h"
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
    Texture* texture1 = new Texture("./textures/container.jpg");

    // Load shader1
    Shader* shader1 = new Shader(
        "MyShader_01",
        "./shaders/default_vertex_shader.shader",
        "./shaders/default_fragment_shader.shader");

    shader1->SetInt("texture1", 0);

    // Create sprite
    Sprite* mySprite = new Sprite("Container", texture1, shader1);

    // Camera transform
    glm::mat4 cameraTransform = glm::mat4(1.0f);

    // My object transform
    glm::mat4 modelTransform = glm::mat4(1.0f);

    while (!GetWindowShouldClose())
    {
        appState.deltaTime = CalculateDeltatime();

        HandleInputEvents();

        // Update shader from camera view
        shader1->SetMat4("camera", cameraTransform);
        shader1->SetMat4("model", modelTransform);

        ClearWindowBuffer(0.2f, 0.3f, 0.3f, 1.0f);

        mySprite->Draw();

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
