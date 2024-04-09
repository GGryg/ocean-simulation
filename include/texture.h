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

    void bind(GLuint unit);
    void unbind(GLuint unit);

    void neareastFilter();
    void bilinearFilter();
    void trilinearFilter();

    void clampToBorder();
    void clampToEdge();
    void repeat();
    void mirrorRepeat();
    
    void activate(GLuint index);

    void allocateStorage(GLuint levels);

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