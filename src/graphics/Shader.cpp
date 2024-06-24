#include "graphics/Shader.h"
#include "graphics/ShaderException.h"

#include <sstream>

namespace gfx
{
	Shader::Shader(const std::string &vertexShaderSource, const std::string &framgnetShaderSource)
		: m_type{ShaderType::normal}
	{
		m_id = glCreateProgram();
		GLuint vertexShader = compile(vertexShaderSource, GL_VERTEX_SHADER);
		GLuint fragmentShader = compile(framgnetShaderSource, GL_FRAGMENT_SHADER);

		glAttachShader(m_id, vertexShader);
		glAttachShader(m_id, fragmentShader);
		glLinkProgram(m_id);

		checkErrors(m_id, ErrorType::program);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	Shader::Shader(const std::string &computeShaderSource)
		: m_type{ShaderType::compute}
	{
		m_id = glCreateProgram();
		GLuint computeShader = compile(computeShaderSource, GL_COMPUTE_SHADER);

		glAttachShader(m_id, computeShader);
		glLinkProgram(m_id);

		glDeleteShader(computeShader);
	}

	void Shader::use() const
	{
		glUseProgram(m_id);
	}

	void Shader::setBool(const std::string &name, bool v) const
	{
		glUniform1i(glGetUniformLocation(m_id, name.c_str()), static_cast<int>(v));
	}

	void Shader::setInt(const std::string &name, int n) const
	{
		glUniform1i(glGetUniformLocation(m_id, name.c_str()), n);
	}

	void Shader::setFloat(const std::string &name, float v) const
	{
		glUniform1f(glGetUniformLocation(m_id, name.c_str()), v);
	}

	void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void Shader::setVec2(const std::string &name, const glm::vec2 &vec) const
	{
		glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &vec[0]);
	}

	void Shader::setVec3(const std::string &name, const glm::vec3 &vec) const
	{
		glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &vec[0]);
	}


	GLuint Shader::compile(const std::string &shaderSource, GLenum type)
	{
		GLuint shader = glCreateShader(type);
		const GLchar *src = shaderSource.data();
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		checkErrors(shader, ErrorType::shader);

		return shader;
	}

	void Shader::checkErrors(GLuint shader, ErrorType type)
	{
		int success{};
		char infoLog[1024];

		if (type == ErrorType::program)
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
				std::stringstream stream;
				stream << "Problem with program linking: " << infoLog;
				throw ShaderException(stream.str());
			}
		}
		else
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
				std::stringstream stream;
				stream << "Problem with shader linking: " << infoLog;
				throw ShaderException(stream.str());
			}
		}
	}
}
