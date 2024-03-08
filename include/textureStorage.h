#ifndef TEXTURE_STORAGE_H
#define TEXTURE_STORAGE_H

#include <glad/gl.h>

#include "texture.h"

class TextureStorage : public Texture
{
public:
    TextureStorage(GLuint width, GLuint height, GLuint levels, GLenum type);
};

#endif