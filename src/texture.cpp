#include <glad/glad.h>

#include "texture.hpp"
#include "file.hpp"
#include "jUtil.hpp"

namespace Texture
{
    unsigned int Create(const std::string &filePath, bool isRGBA)
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

