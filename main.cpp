#include "main.h"

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

GLFWwindow* GetWindow(int width, int height);

void loadOpenGlContext();

void glfwErrorCallback(int errorCode, const char* description);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

std::map<char, Character>* loadCharacters(const char* fontPath);

// Main program start
int main()
{
    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    jAssert(glfwInit() == GLFW_TRUE, "glfwInit() failed");

    // Create window
    GLFWwindow* window = GetWindow(1600, 1200);

    // Load GLAD context
    loadOpenGlContext();

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

void glfwErrorCallback(int errorCode, const char* description)
{
    printf("GLFW error %d: %s\n", errorCode, description);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

std::map<char, Character>* loadCharacters(const char* fontPath)
{
    FT_Library ft;
    FT_Face face;
    FT_Error result;

    std::map<char, Character> characters;

    result = FT_Init_FreeType(&ft);
    jAssert(result == 0, "ERROR::FREETYPE: Could not init FreeType Library");

    result = FT_New_Face(ft, "fonts/arial.ttf", 0, &face);
    jAssert(result == 0, "ERROR::FREETYPE: Failed to load font");

    FT_Set_Pixel_Sizes(face, 0, 48);

    result = FT_Load_Char(face, 'X', FT_LOAD_RENDER);
    jAssert(result == 0, "ERROR::FREETYTPE: Failed to load Glyph");

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        characters.insert(std::pair<char, Character>(c, character));
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return &characters;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

GLFWwindow* GetWindow(int width, int height)
{
    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL!", NULL, NULL);

    jAssert(window != NULL, "glfwCreateWindow() failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback(glfwErrorCallback);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

void loadOpenGlContext()
{
    // Load all OpenGL functions using the glfw loader function
    int loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    jAssert(loaded == 1, "Failed to initialize OpenGL context GLAD");

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);

    printf("Using OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
    printf("Maximum nr of vertex attributes supported: %d\n", nrAttributes);
}