#include "graphics/VBuffer.h"

namespace gfx
{
	VBuffer::VBuffer(const void *data, size_t size, GLenum type) // NOLINT(*-pro-type-member-init)
	{
		glGenBuffers(1, &m_id);
		bind();
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, type);
	}

	VBuffer::~VBuffer()
	{
		glDeleteBuffers(1, &m_id);
	}

	void VBuffer::bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}

	void VBuffer::unbind() const // NOLINT(*-convert-member-functions-to-static)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
