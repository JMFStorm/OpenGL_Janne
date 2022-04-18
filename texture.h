#pragma once

#include <glad/glad.h>

#include "file.h"

class Texture
{
public:
    unsigned int Id = -1;

    bool IsRGBA = false;

    std::string Name;
    std::string FilePath;

    Texture(const std::string &name)
    {
        Name = name;
    }

    unsigned int Init()
    {
        jAssert(FilePath.empty() == false, "FilePath missing, cannot intit texture " + Name);

        unsigned int result = LoadTexture(FilePath, IsRGBA);

        Id = result;

        return result;
    }

    void Bind() 
    {
        jAssert(Id != -1, "Id missing, cannot bind texture " + Name);

        glBindTexture(GL_TEXTURE_2D, Id);
    }

private:
    unsigned int LoadTexture(const std::string &filePath, const bool isGL_RGBA) const
    {
        unsigned int textureId;

        int rbgMode = isGL_RGBA ? GL_RGBA : GL_RGB;

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;

        unsigned char* data = loadImage(filePath.c_str(), &width, &height, &nrChannels);

        jAssert(data, "Failed to load texture: " + filePath);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, rbgMode, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        freeImageData(data);

        return textureId;
    }
};