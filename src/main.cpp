#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>

#pragma warning(pop)

#include "sprite.h"
#include "shader.h"
#include "texture.h"
#include "jUtil.h"
#include "text.h"
#include "window.h"

// Main program start
int main()
{
    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    jAssert(glfwInit() == GLFW_TRUE, "glfwInit() failed");

    GLFWwindow* window = GetWindow(1800, 1200);

    // Load GLAD context
    // Load all OpenGL functions using the glfw loader function
    int loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    jAssert(loaded == 1, "Failed to initialize OpenGL context GLAD");

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);

    printf("Using OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
    printf("Maximum nr of vertex attributes supported: %d\n", nrAttributes);

    // Load text fonts
    std::map<char, Character>* characters = loadCharacters("fonts/arial.ttf");

    // Load shader1
    Shader* shader1 = new Shader();
    shader1->Name = "My Little Shader";
    shader1->VertexFilePath = "./shaders/default_vertex_shader.shader";
    shader1->FragmentFilePath = "./shaders/default_fragment_shader.shader";
    shader1->Init();

    // Load texture1
    Texture* texture1 = new Texture();
    texture1->FilePath = "./textures/container.jpg";
    texture1->IsRGBA = false;
    texture1->Init();

    shader1->SetInt("texture1", 0);

    // Create sprite
    Sprite* mySprite = new Sprite("Container", texture1, shader1);

    // Camera transform
    glm::mat4 cameraTransform = glm::mat4(1.0f);

    // My object transform
    glm::mat4 modelTransform = glm::mat4(1.0f);

    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        float cameraSpeed = 3.5f * deltaTime;

        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
        {
            cameraTransform = glm::translate(cameraTransform, glm::vec3(0.0f, -1.0f, 0.0f) * cameraSpeed);
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
        {
            cameraTransform = glm::translate(cameraTransform, glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed);
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            cameraTransform = glm::translate(cameraTransform, glm::vec3(1.0f, 0.0f, 0.0f) * cameraSpeed);
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            cameraTransform = glm::translate(cameraTransform, glm::vec3(-1.0f, 0.0f, 0.0f) * cameraSpeed);
        }

        // Update shader from camera view
        shader1->SetMat4("camera", cameraTransform);
        shader1->SetMat4("model", modelTransform);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mySprite->Draw();

        glfwSwapBuffers(window);
    }

	return 0;
}
