#pragma once

#include <string>

#include "shader.h"
#include "texture.h"

class Sprite
{
public:
	std::string Name;
    std::string FilePath;

	Texture* MyTexture;
    Shader* MyShader;

    bool IsRGBA = false;

    Sprite(const std::string& name, Texture* texture, Shader* shader);

    void Draw();
};