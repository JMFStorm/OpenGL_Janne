#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>

#include <glm/glm.hpp>

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

std::map<char, Character>* loadCharacters(const char* fontPath);
