#pragma warning(push, 0)

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <ft2build.h>
#include FT_FREETYPE_H

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

#define WINDOW_WIDTH_DEFAULT 1600
#define WINDOW_HEIGHT_DEFAULT 1200

namespace Shader
{
    unsigned int Create(const std::string& vertexFilePath, const std::string& fragmentFilePath);
    void Use(unsigned int shaderId);
}

void JAssert(bool assertion, std::string errorMessage)
{
    if (assertion == false)
    {
        std::string text = errorMessage;
        std::cout << text << std::endl;
        abort();
    }
}

struct FreeTypeCharacter {
    unsigned int textureId;  // ID handle of the glyph texture
    glm::ivec2   size;       // Size of glyph
    glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
    unsigned int advance;    // Offset to advance to next glyph
};

struct FreeTypeFont {
    std::map<GLchar, FreeTypeCharacter> characters;
    int shader;
    int vao;
    int vbo;
} gDebugFTFont;

void LoadFreeTypeFont(const char* fontPath, FreeTypeFont* font)
{
    FT_Library ft;
    FT_Face face;
    FT_Error result;
    std::map<char, FreeTypeCharacter> characterArr;

    result = FT_Init_FreeType(&ft);
    JAssert(result == 0, "ERROR::FREETYPE: Could not init FreeType Library");

    result = FT_New_Face(ft, fontPath, 0, &face);
    JAssert(result == 0, "ERROR::FREETYPE: Failed to load font");

    FT_Set_Pixel_Sizes(face, 0, 24);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 32; c < 128; c++)
    {
        // Load character glyph 
        result = FT_Load_Char(face, c, FT_LOAD_RENDER);

        if (result != 0)
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph from unsigned char: " << c << std::endl;
            continue;
        }

        // Generate texture
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

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Now store character for later use
        FreeTypeCharacter character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        auto newChar = std::pair<char, FreeTypeCharacter>(c, character);
        characterArr.insert(newChar);

        /*
        std::cout 
            << "Loaded unsigned char: '" << c
            << "', Current chars: " << characterArr.size() << "\n";
            */
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int textVAO, textVBO;
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int textShader = Shader::Create(
        "./shaders/font_vertex_shader.shader",
        "./shaders/font_fragment_shader.shader");

    glm::mat4 projection = glm::ortho(
        0.0f,
        (float)WINDOW_WIDTH_DEFAULT,
        0.0f,
        (float)WINDOW_HEIGHT_DEFAULT);

    Shader::Use(textShader);
    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    font->shader = textShader;
    font->vao = textVAO;
    font->vbo = textVBO;
    font->characters = characterArr;
}

std::string ReadFileToString(std::string filePath)
{
    std::stringstream sStream;
    std::ifstream source;
    std::string result;

    // Ensure ifstream objects can throw exceptions:
    source.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        source.open(filePath);
        sStream << source.rdbuf();
        source.close();

        result = sStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        printf("File read failed: %s\n", e.what());
    }

    return result;
}

unsigned char* LoadImage(const char* filePath, int* width, int* height, int* nrChannels)
{
    stbi_set_flip_vertically_on_load(true);
    return stbi_load(filePath, width, height, nrChannels, 0);
}

void FreeImageData(unsigned char* data)
{
    stbi_image_free(data);
}

enum class ShaderDataType
{
    Float,
    Float2,
    Float3,
    Float4,
    Mat3,
    Mat4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool
};

GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float:    return GL_FLOAT;
        case ShaderDataType::Float2:   return GL_FLOAT;
        case ShaderDataType::Float3:   return GL_FLOAT;
        case ShaderDataType::Float4:   return GL_FLOAT;
        case ShaderDataType::Mat3:     return GL_FLOAT;
        case ShaderDataType::Mat4:     return GL_FLOAT;
        case ShaderDataType::Int:      return GL_INT;
        case ShaderDataType::Int2:     return GL_INT;
        case ShaderDataType::Int3:     return GL_INT;
        case ShaderDataType::Int4:     return GL_INT;
        case ShaderDataType::Bool:     return GL_BOOL;
    }

    return 0;
}

namespace VertexArray
{
    unsigned int Create()
    {
        unsigned int vertexArrayObject;

        glGenVertexArrays(1, &vertexArrayObject);

        return vertexArrayObject;
    }

    void Bind(unsigned int vertexArrayObject)
    {
        glBindVertexArray(vertexArrayObject);
    }

    void Unbind()
    {
        glBindVertexArray(0);
    }

    void SetVertexAttributePointer(
        unsigned int attributeIndex,
        int componentsSize,
        ShaderDataType type,
        bool normalize,
        int strideByteSize,
        void* offset)
    {
        unsigned int glType = ShaderDataTypeToOpenGLBaseType(type);
        int setNormalize = normalize ? GL_TRUE : GL_FALSE;

        glEnableVertexAttribArray(attributeIndex);
        glVertexAttribPointer(
            attributeIndex,
            componentsSize,
            glType,
            setNormalize,
            strideByteSize,
            offset);
    }
}

namespace VertexBuffer
{
    unsigned int Create(const std::vector<float> vertices)
    {
        unsigned int vertexBufferObject;

        glGenBuffers(1, &vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

        int sizeInBytes = vertices.size() * sizeof(unsigned int);

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

        int sizeInBytes = indices.size() * sizeof(unsigned int);

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

namespace Shader
{
    unsigned int Create(const std::string& vertexFilePath, const std::string& fragmentFilePath)
    {
        int success;
        char infoLog[512];

        // Get shader source strings
        std::string vertexSourceString = ReadFileToString(vertexFilePath);
        std::string fragmentSourceString = ReadFileToString(fragmentFilePath);

        const char* vertexSource = vertexSourceString.c_str();
        const char* fragmentSource = fragmentSourceString.c_str();

        // Create vertex shader
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        glCompileShader(vertexShader);

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);

        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            printf("Vertex shader compilation failed: %s\n", infoLog);
        }

        // Create fragment shader
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);

        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            printf("Fragment shader compilation failed: %s\n", infoLog);
        }

        // Create new shader program
        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

        if (!success)
        {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            printf("Program linking failed: %s\n", infoLog);
        }

        // Cleanup
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    }

    void Use(unsigned int shaderId)
    {
        glUseProgram(shaderId);
    }

    unsigned int GetUniform(int shaderId, const std::string& name)
    {
        return glGetUniformLocation(shaderId, name.c_str());
    }

    void SetBool(int shaderId, const std::string& name, const bool value)
    {
        glUniform1i(glGetUniformLocation(shaderId, name.c_str()), (int)value);
    }

    void SetInt(int shaderId, const std::string& name, const int value)
    {
        glUniform1i(glGetUniformLocation(shaderId, name.c_str()), value);
    }

    void SetFloat(int shaderId, const std::string& name, const float value)
    {
        glUniform1f(glGetUniformLocation(shaderId, name.c_str()), value);
    }

    void SetVec2(int shaderId, const std::string& name, const glm::vec2& value)
    {
        glUniform2fv(glGetUniformLocation(shaderId, name.c_str()), 1, &value[0]);
    }
    void SetVec2(int shaderId, const std::string& name, const float x, const float y)
    {
        glUniform2f(glGetUniformLocation(shaderId, name.c_str()), x, y);
    }

    void SetVec3(int shaderId, const std::string& name, const glm::vec3& value)
    {
        glUniform3fv(glGetUniformLocation(shaderId, name.c_str()), 1, &value[0]);
    }

    void SetVec3(int shaderId, const std::string& name, float x, float y, float z)
    {
        glUniform3f(glGetUniformLocation(shaderId, name.c_str()), x, y, z);
    }

    void SetVec4(int shaderId, const std::string& name, const glm::vec4& value)
    {
        glUniform4fv(glGetUniformLocation(shaderId, name.c_str()), 1, &value[0]);
    }
    void SetVec4(int shaderId, const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(shaderId, name.c_str()), x, y, z, w);
    }

    void SetMat2(int shaderId, const std::string& name, const glm::mat2& mat)
    {
        glUniformMatrix2fv(glGetUniformLocation(shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void SetMat3(int shaderId, const std::string& name, const glm::mat3& mat)
    {
        glUniformMatrix3fv(glGetUniformLocation(shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void SetMat4(int shaderId, const std::string& name, const glm::mat4& mat)
    {
        glUniformMatrix4fv(glGetUniformLocation(shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
}

namespace Texture
{
    unsigned int Create(const std::string& filePath, bool isRGBA)
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
    void ClearScreenBuffer(float red, float green, float blue, float alpha)
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

    GLFWwindow* Create(bool fullscreen)
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

void RenderFreeTypeText(
    FreeTypeFont* font,
    std::string text,
    float screenX,
    float screenY,
    float scale,
    glm::vec3 color)
{
    // Activate corresponding render state	
    Shader::Use(font->shader);
    Shader::SetVec3(font->shader, "textColor", color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    VertexArray::Bind(font->vao);

    // Iterate through all characters
    std::string::const_iterator current;
    FreeTypeCharacter character;

    float xpos, ypos, cWidth, cHeight;
    const int indicies = 6;

    for (current = text.begin(); current != text.end(); current++)
    {
        character = font->characters[*current];

        xpos = screenX + character.bearing.x * scale;
        ypos = screenY - (character.size.y - character.bearing.y) * scale;

        cWidth = character.size.x * scale;
        cHeight = character.size.y * scale;

        float vertices[indicies * 4] = {
            xpos,           ypos + cHeight,   0.0f, 0.0f,
            xpos,           ypos,             0.0f, 1.0f,
            xpos + cWidth,  ypos,             1.0f, 1.0f,

            xpos,           ypos + cHeight,   0.0f, 0.0f,
            xpos + cWidth,  ypos,             1.0f, 1.0f,
            xpos + cWidth,  ypos + cHeight,   1.0f, 0.0f
        };

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, character.textureId);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, font->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, indicies);

        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        screenX += (character.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    VertexArray::Unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
}

struct FpsCounter
{
    unsigned long frames;
    unsigned int displayFps;
    float previousTimeSecond;
    unsigned int overflowFrames;
    float currentFrameTime;
    float lastFrameTime;
    float deltaTime;
};

struct ApplicationState
{
    FpsCounter fpsCounter;
};

namespace Application
{
    void CalculateDeltatime(FpsCounter* fpsCounter)
    {
        fpsCounter->currentFrameTime = glfwGetTime();
        fpsCounter->deltaTime = fpsCounter->currentFrameTime - fpsCounter->lastFrameTime;
        fpsCounter->lastFrameTime = fpsCounter->currentFrameTime;
    }

    void CalculateFpsScuffed(FpsCounter* fpsCounter)
    {
        fpsCounter->frames++;

        if (1.0f < fpsCounter->currentFrameTime - fpsCounter->previousTimeSecond)
        {
            fpsCounter->displayFps = fpsCounter->frames;
            fpsCounter->frames = 0;
            fpsCounter->previousTimeSecond = fpsCounter->currentFrameTime;
        }
    }

    int Run()
    {
        ApplicationState appState;
        appState.fpsCounter.currentFrameTime = 0.0f;
        appState.fpsCounter.lastFrameTime = 0.0f;
        appState.fpsCounter.deltaTime = 0.0f;
        appState.fpsCounter.frames = 0;
        appState.fpsCounter.displayFps = 0;
        appState.fpsCounter.previousTimeSecond = 1.0f;
        appState.fpsCounter.overflowFrames = 0;

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
        LoadFreeTypeFont("fonts/Roboto-Regular.ttf", &gDebugFTFont);

        // Load texture1
        unsigned int texture1 = Texture::Create("./images/container.jpg", false);

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

        while (!Window::ShouldClose(window))
        {
            Window::HandleInputEvents(window);

            Window::ClearScreenBuffer(0.2f, 0.3f, 0.3f, 1.0f);

            Texture::Bind(texture1);
            Shader::Use(shader1);

            glActiveTexture(GL_TEXTURE0);
            VertexArray::Bind(vertexArrayObject);

            int elementsCount = indices.size();
            glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, 0);

            std::stringstream stream;
            stream << std::fixed << std::setprecision(2) << appState.fpsCounter.deltaTime * 1000;
            auto displayDelta = "Delta: " + stream.str() + "ms";

            stream.str("");
            stream << std::fixed << std::setprecision(1) << appState.fpsCounter.currentFrameTime;
            auto displayCurrent = "Time: " + stream.str() + "s";

            stream.str("");
            stream << std::fixed << std::setprecision(1) << appState.fpsCounter.displayFps;
            auto displayFps = "FPS: " + stream.str();

            RenderFreeTypeText(
                &gDebugFTFont,
                displayDelta,
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

            RenderFreeTypeText(
                &gDebugFTFont,
                displayFps,
                1350.0f,
                1100.0f,
                1.0f,
                glm::vec3(0.8, 0.8f, 0.8f));

            Window::SwapScreenBuffer(window);

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
