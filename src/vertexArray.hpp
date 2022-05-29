#pragma once

#include "shaderDataType.hpp"

namespace VertexArray
{
	unsigned int Create();

	void Bind(unsigned int vertexArrayObject);

	void Unbind();

	void SetVertexAttributePointer(
		unsigned int attributeIndex,
		int componentsSize,
		ShaderDataType type,
		bool normalize,
		int strideByteSize,
		void* offset);
}
