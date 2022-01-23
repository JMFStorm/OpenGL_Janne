#pragma once

#include <string>
#include <glad/glad.h>

#include "file.h"

unsigned int createShaderProgram(const char* vertexFilePath, const char* fragmentFilePath);
