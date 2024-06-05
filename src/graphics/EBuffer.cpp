#include "graphics/EBuffer.h"

namespace gfx
{
	EBuffer::EBuffer(const GLuint *data, size_t count, GLenum type)
		: m_id(0)
		, m_count{static_cast<GLuint>(count)}
	{
		glGenBuffers(1, &m_id);
		bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(count * sizeof(GLuint)), data, type);
	}

	EBuffer::~EBuffer()
	{
		glDeleteBuffers(1, &m_id);
	}

	void EBuffer::bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}

	void EBuffer::unbind() const // NOLINT(*-convert-member-functions-to-static)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	GLuint EBuffer::count() const
	{
		return m_count;
	}
}
