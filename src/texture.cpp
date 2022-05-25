#include "texture.h"

#include <glad/glad.h>

#include "file.h"
#include "jUtil.h"

Texture::Texture()
{
}

void Texture::Bind()
{
    jAssert(Id != 0, "Id missing, cannot bind texture " + FilePath);
    glBindTexture(GL_TEXTURE_2D, Id);
}

unsigned int Texture::Init()
{
    jAssert(FilePath.empty() == false, "FilePath missing, cannot intit texture ");

    unsigned int textureId;
    int rbgMode = IsRGBA ? GL_RGBA : GL_RGB;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;

    unsigned char* data = loadImage(FilePath.c_str(), &width, &height, &nrChannels);

    jAssert(data, "Failed to load texture: " + FilePath);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, rbgMode, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    freeImageData(data);

    Id = textureId;

    return textureId;
}