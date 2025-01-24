#include "graphics/Texture.h"

namespace gfx
{
	Texture::Texture(GLenum target, GLuint width, GLuint height, GLenum internalFormat, GLenum imageFormat)
		: m_id(0)
		, m_target(target)
		, m_width(width)
		, m_height(height)
		, m_nrChannels(0)
		, m_internalFormat(internalFormat)
		, m_imageFormat(imageFormat)
	{
		glGenTextures(1, &m_id);
	}

	Texture::Texture(GLenum target, GLuint width, GLuint height, GLuint nrChannels, GLenum internalFormat,
	                 GLenum imageFormat)
		: m_id(0)
		, m_target(target)
		, m_width(width)
		, m_height(height)
		, m_nrChannels(nrChannels)
		, m_internalFormat(internalFormat)
		, m_imageFormat(imageFormat)
	{
		glGenTextures(1, &m_id);
	}


	Texture::Texture(GLenum target, GLuint width, GLuint height)
		: m_target(target)
		, m_width(width)
		, m_height(height)
	{
		glGenTextures(1, &m_id);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_id);
	}

	void Texture::generate(const unsigned char *data) const
	{
		glBindTexture(m_target, m_id);
		glTexImage2D(m_target, 0, m_internalFormat, m_width, m_height, 0, m_imageFormat, GL_UNSIGNED_BYTE, data);
		glBindTexture(m_target, 0);
	}

	void Texture::bindActive(GLuint unit) const
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(m_target, m_id);
	}

	void Texture::bind() const
	{
		glBindTexture(m_target, m_id);
	}

	void Texture::bindImage(GLuint unit, GLuint levels, GLuint layer, GLenum access) const
	{
		bindActive(unit);
		glBindImageTexture(unit, m_id, levels, GL_FALSE, 0, access, m_internalFormat);
	}


	void Texture::unbindActive(GLuint unit) const
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(m_target, 0);
	}

	void Texture::unbind() const
	{
		glBindTexture(m_target, 0);
	}

	void Texture::nearestFilter() const
	{
		glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	void Texture::bilinearFilter() const
	{
		glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	void Texture::trilinearFilter() const
	{
		glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(m_target);
		glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	void Texture::clampToBorder()
	{
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	}

	void Texture::clampToEdge() const
	{
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	void Texture::clampToEdgeCube() const
	{
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void Texture::repeat() const
	{
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	void Texture::mirrorRepeat() const
	{
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}

	void Texture::allocateStorage(GLuint levels) const
	{
		glTexStorage2D(m_target, levels, m_internalFormat, m_width, m_height);
	}

	void Texture::texImageCubemap(GLenum target, const unsigned char *data) const
	{
		glTexImage2D(target, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}

	void Texture::setWidth(GLuint width)
	{
		m_width = width;
	}

	void Texture::setHeight(GLuint height)
	{
		m_height = height;
	}

	GLuint Texture::id() const
	{
		return m_id;
	}
}
