#include "text.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "jUtil.h"

#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

std::map<char, Character>* loadCharacters(const char* fontPath)
{
    FT_Library ft;
    FT_Face face;
    FT_Error result;

    std::map<char, Character> characters;

    result = FT_Init_FreeType(&ft);
    jAssert(result == 0, "ERROR::FREETYPE: Could not init FreeType Library");

    result = FT_New_Face(ft, "fonts/arial.ttf", 0, &face);
    jAssert(result == 0, "ERROR::FREETYPE: Failed to load font");

    FT_Set_Pixel_Sizes(face, 0, 48);

    result = FT_Load_Char(face, 'X', FT_LOAD_RENDER);
    jAssert(result == 0, "ERROR::FREETYTPE: Failed to load Glyph");

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        characters.insert(std::pair<char, Character>(c, character));
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return &characters;
}