#ifndef SHADEREXCEPTION_H
#define SHADEREXCEPTION_H

#include <stdexcept>

namespace gfx
{
	class ShaderException : public std::runtime_error
	{
	public:
		ShaderException(const std::string &error)
			: std::runtime_error{error}
		{
		}
	};
}

#endif //SHADEREXCEPTION_H
