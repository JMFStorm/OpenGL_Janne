#pragma once

namespace VertexArray
{
	unsigned int Create();

	void Bind(unsigned int vertexArrayObject);

	void Unbind();

	void InitHardcodedVertexAttributes();
}
