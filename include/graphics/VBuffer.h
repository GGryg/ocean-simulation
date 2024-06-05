#ifndef VBUFFER_H
#define VBUFFER_H

#include <glad/gl.h>
#include <memory>

namespace gfx
{
	// Class abstracting vertex buffer
	class VBuffer
	{
	public:
		VBuffer(const void *data, size_t size, GLenum type);

		~VBuffer();

		void bind() const;

		void unbind() const;

	private:
		GLuint m_id;
	};

	using VBufferPtr = std::unique_ptr<VBuffer>;
}

#endif //VBUFFER_H
