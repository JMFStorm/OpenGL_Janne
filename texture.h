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

    Texture();

    void Bind();
    unsigned int Init();
};