#include "vertexArray.hpp"

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

    void SetVertexAttributePointer(
        unsigned int attributeIndex,
        int componentsSize,
        ShaderDataType type,
        bool normalize,
        int strideByteSize,
        void* offset)
    {
        unsigned int glType = ShaderDataTypeToOpenGLBaseType(type);
        int setNormalize = normalize ? GL_TRUE : GL_FALSE;

        glEnableVertexAttribArray(attributeIndex);
        glVertexAttribPointer(
            attributeIndex,
            componentsSize,
            glType,
            setNormalize,
            strideByteSize,
            offset);
    }
}