#pragma once

#include <string>

namespace Texture
{
    int Create(const std::string& filePath, bool isRGBA);

    void Bind(int textureId);
}