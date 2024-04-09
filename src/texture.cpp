#include "texture.h"

Texture::Texture(GLenum target, GLuint width, GLuint height, GLenum internalFormat, GLenum imageFormat)
    : m_target(target)
    , m_width(width)
    , m_height(height)
    , m_internalFormat(internalFormat)
    , m_imageFormat(imageFormat)
{
    glGenTextures(1, &m_id);
}

Texture::Texture(GLenum target, GLuint width, GLuint height, GLuint nrChannels, GLenum internalFormat, GLenum imageFormat)
    : m_target(target)
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

void Texture::generate(unsigned char* data)
{
    glBindTexture(m_target, m_id);
    glTexImage2D(m_target, 0, m_internalFormat, m_width, m_height, 0, m_imageFormat, GL_UNSIGNED_BYTE, data);
    glBindTexture(m_target, 0);
}

void Texture::bind(GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(m_target, m_id);
}

void Texture::unbind(GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(m_target, 0);
}

void Texture::neareastFilter()
{
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void Texture::bilinearFilter()
{
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture::trilinearFilter()
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

void Texture::clampToEdge()
{
    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Texture::repeat()
{
    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::mirrorRepeat()
{
    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
}

void Texture::activate(GLuint index)
{
    glActiveTexture(GL_TEXTURE0 + index);
}

void Texture::allocateStorage(GLuint levels)
{
    glTexStorage2D(m_target, levels, m_internalFormat, m_width, m_height);
}