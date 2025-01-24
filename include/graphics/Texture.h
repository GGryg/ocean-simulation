#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <glad/gl.h>

namespace gfx
{
	class Texture
	{
	public:
		Texture(GLenum target, GLuint width, GLuint height, GLenum internalFormat, GLenum imageFormat);//

		Texture(GLenum target, GLuint width, GLuint height, GLuint nrChannels, GLenum internalFormat,
		        GLenum imageFormat);

		Texture(GLenum target, GLuint width, GLuint height);

		~Texture();

		void generate(const unsigned char *data) const;

		void bindActive(GLuint unit) const;

		void bind() const;

		void bindImage(GLuint unit, GLuint levels, GLuint layer, GLenum access) const;

		void unbindActive(GLuint unit) const;

		void unbind() const;

		void nearestFilter() const;

		void bilinearFilter() const;

		void trilinearFilter() const;

		void clampToBorder();

		void clampToEdge() const;

		void clampToEdgeCube() const;

		void repeat() const;

		void mirrorRepeat() const;

		void allocateStorage(GLuint levels) const;

		void texImageCubemap(GLenum target, const unsigned char *data) const;

		void setWidth(GLuint width);

		void setHeight(GLuint height);

		GLuint id() const;

	private:
		GLuint m_id;
		GLenum m_target;
		GLuint m_width{};
		GLuint m_height{};
		GLuint m_nrChannels;
		GLenum m_internalFormat;
		GLenum m_imageFormat;
	};

	using TexturePtr = std::unique_ptr<Texture>;
}

#endif //TEXTURE_H
