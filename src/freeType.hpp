#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <string>
#include <map>

namespace FreeType
{
    struct Character {
        unsigned int textureId;  // ID handle of the glyph texture
        glm::ivec2   size;       // Size of glyph
        glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
        unsigned int advance;    // Offset to advance to next glyph
    };

    struct Font {
        std::map<GLchar, FreeType::Character> characters;
        int shader;
        int vao;
        int vbo;
    };

    void LoadFont(const char* fontPath, FreeType::Font* font);

    void RenderText(
        FreeType::Font* font,
        std::string text,
        float screenX,
        float screenY,
        float scale,
        glm::vec3 color);
}