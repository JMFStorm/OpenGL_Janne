#include "buffer.hpp"

#include <glad/glad.h>

#include "jUtil.hpp"

namespace VertexBuffer
{
	unsigned int Create(const std::vector<float> vertices)
	{
		unsigned int vertexBufferObject;

		glGenBuffers(1, &vertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

		int sizeInBytes = vertices.size() * sizeof(unsigned int);

		glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &vertices[0], GL_STATIC_DRAW);

		GLint size = 0;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

		if (sizeInBytes != size)
		{
			glDeleteBuffers(1, &vertexBufferObject);
			JAssert(sizeInBytes != size, "VertexBufferObject GL_BUFFER_SIZE failed");
		}

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

		GLint size = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

		if (sizeInBytes != size)
		{
			glDeleteBuffers(1, &indexBufferObject);
			JAssert(sizeInBytes != size, "IndexBufferObject GL_BUFFER_SIZE failed");
		}

		return indexBufferObject;
	}
}