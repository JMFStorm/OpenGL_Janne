#pragma once

#include <vector>

namespace VertexBuffer
{
	unsigned int Create(const std::vector<float> vertices);
}

namespace IndexBuffer
{
	unsigned int Create(const std::vector<unsigned int> indices);
}