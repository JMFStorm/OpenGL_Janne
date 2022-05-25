#pragma once

#include <string>

#include "shader.h"
#include "texture.h"

class Sprite
{
public:
	std::string Name;
    Texture* MyTexture;
    Shader* MyShader;

    Sprite(const std::string& name, Texture* texture, Shader* shader);

    void Draw();
};