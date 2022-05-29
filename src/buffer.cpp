#include "buffer.hpp"

#include <glad/glad.h>

namespace VertexBuffer
{
	unsigned int Create(const std::vector<float> vertices)
	{
		unsigned int vertexBufferObject;

		glGenBuffers(1, &vertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

		int sizeInBytes = vertices.size() * sizeof(unsigned int);

		glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &vertices[0], GL_STATIC_DRAW);

		return vertexBufferObject;
	}
}

namespace IndexBuffer
{
	unsigned int Create(const std::vector<unsigned int> indices)
	{
		unsigned int indexBufferObject;

		glGenBuffers(1, &indexBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);

		int sizeInBytes = indices.size() * sizeof(unsigned int);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeInBytes, &indices[0], GL_STATIC_DRAW);

		return indexBufferObject;
	}
}