#ifndef EBUFFER_H
#define EBUFFER_H

#include <glad/gl.h>
#include <memory>

namespace gfx
{
	// Class abstracting element buffer
	class EBuffer
	{
	public:
		EBuffer(const GLuint *data, size_t count, GLenum type);

		~EBuffer();

		void bind() const;

		void unbind() const;

		GLuint count() const;

	private:
		GLuint m_id;
		GLuint m_count;
	};

	using EBufferPtr = std::unique_ptr<EBuffer>;
}
#endif //EBUFFER_H
