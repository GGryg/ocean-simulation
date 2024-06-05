#ifndef VARRAY_H
#define VARRAY_H

#include <vector>
#include <memory>

#include "VBuffer.h"

namespace gfx
{
	// Element in vertex buffer
	struct VBufferElement
	{
		GLuint count;
		GLenum type;
		GLenum normalized;
		GLuint offset;
	};

	using VBufferLayout = std::vector<VBufferElement>;

	// Class abstracting vertex array
	class VArray
	{
	public:
		VArray();

		~VArray();

		void addBuffer(const VBuffer *vb, size_t stride, const VBufferLayout &layout) const;

		void bind() const;

		void unbind() const;

	private:
		GLuint m_id;
	};

	using VArrayPtr = std::unique_ptr<VArray>;
}
#endif //VARRAY_H
