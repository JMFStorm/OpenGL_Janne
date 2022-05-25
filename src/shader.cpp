#include "shader.hpp"

Shader::Shader(
    std::string name,
    std::string vertexFilePath,
    std::string fragmentFilePath)
{
    Name = name;
    VertexFilePath = vertexFilePath;
    FragmentFilePath = fragmentFilePath;

    jAssert(VertexFilePath.empty() == false, "VertexFilePath missing, cannot compile " + Name);
    jAssert(FragmentFilePath.empty() == false, "FragmentFilePath missing, cannot compile " + Name);

    glGenVertexArrays(1, &VertexArrayObject);
    glGenBuffers(1, &VertexBufferObject);
    glGenBuffers(1, &ElementBufferObject);

    glBindVertexArray(VertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Texture attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    jAssert(VertexArrayObject != 0, "VertexArrayObject isn't initialized, cannot init sprite " + Name);
    jAssert(VertexBufferObject != 0, "VertexBufferObject isn't initialized, cannot init sprite " + Name);
    jAssert(ElementBufferObject != 0, "ElementBufferObject isn't initialized, cannot init sprite " + Name);

    unsigned int shaderId = CreateShaderProgram(VertexFilePath, FragmentFilePath);

    Id = shaderId;
}

void Shader::Use() const
{
    jAssert(Id != -1, "Id isn't initialized, cannot use shader " + Name);
    glUseProgram(Id);
}

void Shader::DrawElements()
{
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VertexArrayObject);
    glDrawElements(GL_TRIANGLES, sizeof(Vertices), GL_UNSIGNED_INT, 0);
}

unsigned int Shader::GetUniform(const std::string& name) const
{
    return glGetUniformLocation(Id, name.c_str());
}

void Shader::SetBool(const std::string& name, const bool value) const
{
    glUniform1i(glGetUniformLocation(Id, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, const int value) const
{
    glUniform1i(glGetUniformLocation(Id, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, const float value) const
{
    glUniform1f(glGetUniformLocation(Id, name.c_str()), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(Id, name.c_str()), 1, &value[0]);
}
void Shader::SetVec2(const std::string& name, const float x, const float y) const
{
    glUniform2f(glGetUniformLocation(Id, name.c_str()), x, y);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(Id, name.c_str()), 1, &value[0]);
}

void Shader::SetVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(Id, name.c_str()), x, y, z);
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(Id, name.c_str()), 1, &value[0]);
}
void Shader::SetVec4(const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(Id, name.c_str()), x, y, z, w);
}

void Shader::SetMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

unsigned int Shader::CreateShaderProgram(const std::string& vertexFilePath, const std::string& fragmentFilePath) const
{
    int success;
    char infoLog[512];

    // Get shader source strings
    std::string vertexSourceString = readFileToString(vertexFilePath);
    std::string fragmentSourceString = readFileToString(fragmentFilePath);

    const char* vertexSource = vertexSourceString.c_str();
    const char* fragmentSource = fragmentSourceString.c_str();

    // Create vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex shader compilation failed: %s\n", infoLog);
    }

    // Create fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment shader compilation failed: %s\n", infoLog);
    }

    // Create new shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Program linking failed: %s\n", infoLog);
    }

    // Cleanup
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
