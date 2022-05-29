#pragma once

#include <glad/glad.h>

enum class ShaderDataType
{
	Float,
	Float2,
	Float3,
	Float4,
	Mat3,
	Mat4,
	Int,
	Int2,
	Int3,
	Int4,
	Bool
};

GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type);
