#pragma once

#include <string>

class Texture
{
public:
    unsigned int Id = 0;
    bool IsRGBA = false;

    std::string FilePath;

    Texture(std::string filePath);

    void Bind();
};