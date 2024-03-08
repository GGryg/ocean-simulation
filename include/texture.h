#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/gl.h>

class Texture
{
public:
    Texture(GLenum target, GLuint width, GLuint height, GLuint internalFormat, GLuint imageFormat);
    Texture(GLenum target, GLuint width, GLuint height);
    ~Texture();

    void generate();

    void bind();
    void unbind();

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
    GLuint m_internalFormat;
    GLuint m_imageFormat;
}

#endif