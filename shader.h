#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "file.h"
#include "jUtil.h"

class Shader
{
public:
    unsigned int Id;

    std::string Name;
    std::string VertexFilePath = "";
    std::string FragmentFilePath = "";

    glm::vec3 Color = { 1.0f, 1.0f, 1.0f };

    Shader(std::string shaderName);

    unsigned int Init();

    void Use() const;

    void DrawElements();

    unsigned int GetUniform(const std::string& name) const;

    void SetBool(const std::string& name, const bool value) const;

    void SetInt(const std::string& name, const int value) const;

    void SetFloat(const std::string& name, const float value) const;

    void SetVec2(const std::string& name, const glm::vec2& value) const;
    void SetVec2(const std::string& name, const float x, const float y) const;

    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec3(const std::string& name, float x, float y, float z) const;

    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetVec4(const std::string& name, float x, float y, float z, float w);

    void SetMat2(const std::string& name, const glm::mat2& mat) const;
    void SetMat3(const std::string& name, const glm::mat3& mat) const;

    void SetMat4(const std::string& name, const glm::mat4& mat) const;

private:
    unsigned int VertexArrayObject = 0;
    unsigned int VertexBufferObject = 0;
    unsigned int ElementBufferObject = 0;

    unsigned int Indices[3 * 2] = {
        0, 1, 2, // first triangle
        2, 3, 0  // second triangle
    };

    float Vertices[5 * 4] = {
        // positions         // texture coords
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, // top left 
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f  // bottom left
    };

    unsigned int CreateShaderProgram(const std::string& vertexFilePath, const std::string& fragmentFilePath) const;
};
