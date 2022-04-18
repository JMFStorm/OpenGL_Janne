#pragma once

#include <glad/glad.h>

#include "file.h"
#include "jUtil.h"

class Texture
{
public:
    unsigned int Id = 0;

    bool IsRGBA = false;

    std::string FilePath;

    Texture(const std::string& filePath);

    void Bind();

private:
    unsigned int LoadTexture(const std::string& filePath, const bool isGL_RGBA) const;
};