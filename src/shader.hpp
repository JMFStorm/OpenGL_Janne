#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "file.hpp"
#include "jUtil.hpp"

namespace Shader
{
    unsigned int Create(
        const std::string& vertexFilePath,
        const std::string& fragmentFilePath);

    void Use(unsigned int shaderId);

    unsigned int GetUniform(int shaderId, const std::string& name);

    void SetBool(int shaderId, const std::string& name, const bool value);

    void SetInt(int shaderId, const std::string& name, const int value);

    void SetFloat(int shaderId, const std::string& name, const float value);

    void SetVec2(int shaderId, const std::string& name, const glm::vec2& value);
    void SetVec2(int shaderId, const std::string& name, const float x, const float y);

    void SetVec3(int shaderId, const std::string& name, const glm::vec3& value);
    void SetVec3(int shaderId, const std::string& name, float x, float y, float z);

    void SetVec4(int shaderId, const std::string& name, const glm::vec4& value);
    void SetVec4(int shaderId, const std::string& name, float x, float y, float z, float w);

    void SetMat2(int shaderId, const std::string& name, const glm::mat2& mat);
    void SetMat3(int shaderId, const std::string& name, const glm::mat3& mat);

    void SetMat4(int shaderId, const std::string& name, const glm::mat4& mat);
};
