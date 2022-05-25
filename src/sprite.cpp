#include "sprite.h"

Sprite::Sprite(const std::string& name, Texture* texture, Shader* shader)
{
    Name = name;
    MyTexture = texture;
    MyShader = shader;
}

void Sprite::Draw()
{
    MyTexture->Bind();
    MyShader->Use();
    MyShader->DrawElements();
}
