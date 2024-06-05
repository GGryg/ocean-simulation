#ifndef SHADER_H
#define SHADER_H

#include <memory>
#include <glad/gl.h>
#include <string>
#include <glm/glm.hpp>

namespace gfx
{
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
		Shader(const std::string &vertexShaderSource, const std::string &framgnetShaderSource);

		Shader(const std::string &computeShaderSource);

		void use() const;

		bool isValid() const;

		void setBool(const std::string &name, bool v) const;

		void setInt(const std::string &name, int n) const;

		void setFloat(const std::string &name, float v) const;

		void setMat4(const std::string &name, const glm::mat4 &mat) const;

		void setVec2(const std::string &name, const glm::vec2 &vec) const;

		void setVec3(const std::string &name, const glm::vec3 &vec) const;

	private:
		GLuint compile(const std::string &source, GLenum type);

		void checkErrors(GLuint shader, ErrorType type);

		GLuint m_id{};
		ShaderType m_type{};
		bool m_valid;
	};

	using ShaderPtr = std::unique_ptr<Shader>;
}

#endif //SHADER_H
