#pragma once

#include <string>

namespace Texture
{
    unsigned int Create(const std::string& filePath, bool isRGBA);

    void Bind(unsigned int textureId);
}