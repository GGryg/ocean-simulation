#ifndef TEXTUREEXCEPTION_H
#define TEXTUREEXCEPTION_H

#include <stdexcept>

namespace gfx
{
	class TextureException : public std::runtime_error
	{
	public:
		TextureException(const std::string &error)
			: std::runtime_error{"TEXTURE: " + error}
		{
		}
	};
}

#endif //TEXTUREEXCEPTION_H
