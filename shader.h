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

    Shader(std::string shaderName)
    {
        Name = shaderName;
    }

    unsigned int Compile()
    {
        jAssert(VertexFilePath.empty() == false, "VertexFilePath missing, cannot compile " + Name);
        jAssert(FragmentFilePath.empty() == false, "FragmentFilePath missing, cannot compile " + Name);

        unsigned int shaderId = CreateShaderProgram(VertexFilePath, FragmentFilePath);

        Id = shaderId;

        return shaderId;
    }

    void Use()
    {
        glUseProgram(Id);
    }

    unsigned int GetUniform(const std::string& name)
    {
        return glGetUniformLocation(Id, name.c_str());
    }

    void SetBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(Id, name.c_str()), (int)value);
    }

    void SetInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(Id, name.c_str()), value);
    }

    void SetFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(Id, name.c_str()), value);
    }

    void SetVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(Id, name.c_str()), 1, &value[0]);
    }
    void SetVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(Id, name.c_str()), x, y);
    }

    void SetVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(Id, name.c_str()), 1, &value[0]);
    }

    void SetVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(Id, name.c_str()), x, y, z);
    }

    void SetVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(Id, name.c_str()), 1, &value[0]);
    }
    void SetVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(Id, name.c_str()), x, y, z, w);
    }

    void SetMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void SetMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void SetMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    unsigned int CreateShaderProgram(std::string vertexFilePath, std::string fragmentFilePath)
    {
        int success;
        char infoLog[512];

        // Get shader source strings
        std::string vertexSourceString = readFileToString(vertexFilePath);
        std::string fragmentSourceString = readFileToString(fragmentFilePath);

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
};
