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

    unsigned int VertexArrayObject = -1;
    unsigned int VertexBufferObject = -1;
    unsigned int ElementBufferObject = -1;

	Sprite(Texture* texture, Shader* shader)
	{
        Name = "sprite_" + texture->Name;
        MyTexture = texture;
        MyShader = shader;
	}

    void Init()
    {
        jAssert(MyTexture != NULL, "MyTexture is null, cannot init sprite " + Name);
        jAssert(MyShader != NULL, "MyShader is null, cannot init sprite " + Name);

        glGenVertexArrays(1, &VertexArrayObject);
        glGenBuffers(1, &VertexBufferObject);
        glGenBuffers(1, &ElementBufferObject);

        glBindVertexArray(VertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

        // Color attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

        // Texture attribute
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

        jAssert(VertexArrayObject != -1, "VertexArrayObject isn't initialized, cannot init sprite " + Name);
        jAssert(VertexBufferObject != -1, "VertexBufferObject isn't initialized, cannot init sprite " + Name);
        jAssert(ElementBufferObject != -1, "ElementBufferObject isn't initialized, cannot init sprite " + Name);
    }

    void Draw()
    {
        MyShader->Use();
        MyTexture->Bind();

        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(VertexArrayObject);
        glDrawElements(GL_TRIANGLES, sizeof(vertices), GL_UNSIGNED_INT, 0);
    }

private:
    float vertices[8 * 4] = {
        // positions         // color           // texture coords
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f  // top left 
    };

    unsigned int indices[3 * 2] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
};