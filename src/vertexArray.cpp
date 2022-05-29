#include "vertexArray.hpp"

#include <glad/glad.h>

namespace VertexArray
{
	unsigned int Create()
	{
		unsigned int vertexArrayObject;

		glGenVertexArrays(1, &vertexArrayObject);

		return vertexArrayObject;
	}

	void Bind(unsigned int vertexArrayObject)
	{
		glBindVertexArray(vertexArrayObject);
	}

	void Unbind()
	{
		glBindVertexArray(0);
	}

	void InitHardcodedVertexAttributes()
	{
		// Position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

		// Texture attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
}
