//
// Created by Shiryon on 28.04.2024.
//

#ifndef SHADEREXCEPTION_H
#define SHADEREXCEPTION_H

#include <stdexcept>

class ShaderException : public std::runtime_error
{
public:
	ShaderException(const std::string& error)
		: std::runtime_error{error}
	{
	}
};

#endif //SHADEREXCEPTION_H
