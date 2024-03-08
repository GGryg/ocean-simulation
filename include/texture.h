#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/gl.h>

class Texture
{
public:
    Texture(GLuint width, GLuint height, GLuint internalFormat, GLuint imageFormat, GLuint wrapS, GLuint wrapT, GLuint filterMin, GLuint filterMax);

    void bind();
    void unbind();
    
private:
    GLuint m_id;
    GLuint m_width{};
    GLuint m_height{};
    GLuint m_internalFormat;
    GLuint m_imageFormat;
    GLuint m_wrapS;
    GLuint m_wrapT;
    GLuint m_filterMin;
    GLuint m_filterMax;
}

#endif