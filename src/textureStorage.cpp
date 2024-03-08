#include "textureStorage.h"

TextureStorage::TextureStorage(GLuint width, GLuint height, GLuint levels, GLenum type)
    : Texture(GL_TEXTURE_2D, width, height)
{
    bind();

    
}