#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include <glad/gl.h>

class Texture
{
public:
    Texture(GLenum target, GLuint width, GLuint height, GLenum internalFormat, GLenum imageFormat);
    Texture(GLenum target, GLuint width, GLuint height, GLuint nrChannels, GLenum internalFormat, GLenum imageFormat);
    Texture(GLenum target, GLuint width, GLuint height);
    ~Texture();

    void generate(unsigned char* data);

    void bindActive(GLuint unit) const;
    void bind() const;
    void bindImage(GLuint unit, GLuint levels, GLuint layer, GLenum access) const;
    void unbindActive(GLuint unit) const;
    void unbind() const;

    void neareastFilter();
    void bilinearFilter();
    void trilinearFilter();

    void clampToBorder();
    void clampToEdge();
    void clampToEdgeCube();
    void repeat();
    void mirrorRepeat();
    
    void activate(GLuint unit) const;

    void allocateStorage(GLuint levels);
    void texImage2D();
    void texImageCubemap(GLenum target, unsigned char* data);

    void setWidth(GLuint width);
    void setHeight(GLuint height);

private:
    GLuint m_id;
    GLenum m_target;
    GLuint m_width{};
    GLuint m_height{};
    GLuint m_nrChannels;
    GLenum m_internalFormat;
    GLenum m_imageFormat;
};

#endif