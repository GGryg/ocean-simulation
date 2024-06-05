#include "graphics/VArray.h"

namespace gfx
{
	VArray::VArray() // NOLINT(*-pro-type-member-init)
	{
		glGenVertexArrays(1, &m_id);
	}

	VArray::~VArray()
	{
		glDeleteVertexArrays(1, &m_id);
	}

	void VArray::addBuffer(const VBuffer *vb, size_t stride, const VBufferLayout &elements) const
	{
		bind();
		vb->bind();
		for (GLuint i = 0; i < elements.size(); ++i)
		{
			const VBufferElement element = elements[i];
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, static_cast<GLint>(element.count), element.type, GL_FALSE,
			                      static_cast<GLsizei>(stride), reinterpret_cast<const void *>(element.offset));
		}
	}

	void VArray::bind() const
	{
		glBindVertexArray(m_id);
	}

	void VArray::unbind() const // NOLINT(*-convert-member-functions-to-static)
	{
		glBindVertexArray(0);
	}
}
