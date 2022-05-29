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
}
