#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h>
#include <string>
#include <glm/glm.hpp>

enum class ShaderType
{
    normal,
    compute
};

enum class ErrorType
{
    shader,
    program
};

class Shader
{
public:
    Shader(const std::string& vertexShaderSource, const std::string& framgnetShaderSource);
    Shader(const std::string& computeShaderSource);
    Shader() {}

    void use() const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    GLuint compile(const std::string& source, GLenum type);
    void checkErrors(GLuint shader, ErrorType type);

    GLuint m_id{};
    ShaderType m_type{};
};

#endif