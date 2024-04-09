#include "resourceLoader.h"
#include "logger.h"
#include <fstream>
#include <sstream>

#include <glad/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Shader ResourceLoader::loadShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{  
    Logger::get().log("Loading vertex and fragment shader from files: " + vertexShaderFile + ", " + fragmentShaderFile);
    return loadShaderFromFile(vertexShaderFile, fragmentShaderFile);
}

Shader ResourceLoader::loadShader(const std::string& computeShaderFile)
{
    Logger::get().log("Loading compute shader from file: " + computeShaderFile);
    return loadComputeShaderFromFile(computeShaderFile);
}

Shader ResourceLoader::loadShaderFromFile(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{
    std::string vertexShaderSource;
    std::string fragmentShaderSource;

    try
    {
        std::ifstream vertexShaderF(vertexShaderFile);
        std::ifstream fragmentShaderF(fragmentShaderFile);
        std::stringstream vertexShaderStream;
        std::stringstream fragmnetShaderStream;

        vertexShaderStream << vertexShaderF.rdbuf();
        fragmnetShaderStream << fragmentShaderF.rdbuf();

        vertexShaderF.close();
        fragmentShaderF.close();

        vertexShaderSource = vertexShaderStream.str();
        fragmentShaderSource = fragmnetShaderStream.str();
    }
    catch(std::exception& ex)
    {
        Logger::get().log("SHADER: Failed to read shader files", true);
        Logger::get().log(ex.what(), true);
    }

    return Shader{vertexShaderSource, fragmentShaderSource};
}

Shader ResourceLoader::loadComputeShaderFromFile(const std::string& computeShaderFile)
{
    std::string computeShaderSource;

    try
    {
        std::ifstream computeShaderF(computeShaderFile);
        std::stringstream computeShaderStream;

        computeShaderStream << computeShaderF.rdbuf();

        computeShaderF.close();

        computeShaderSource = computeShaderStream.str();
    }
    catch(std::exception& ex)
    {
        Logger::get().log("COMPUTE:SHADER: Failed to read shader files", true);
        Logger::get().log(ex.what(), true);
    }

    return Shader{computeShaderSource};
}

Texture* ResourceLoader::loadTexture(const std::string& textureFile, bool alpha)
{
    Logger::get().log("Loading texture from file: " + textureFile);
    return loadTextureFromFile(textureFile, alpha);
}

Texture* ResourceLoader::loadTextureFromFile(const std::string& textureFile, bool alpha)
{
    GLint width;
    GLint height;
    GLint nrChannels;
    unsigned char* data = stbi_load(textureFile.c_str(), &width, &height, &nrChannels, 0);
    if(!data)
    {
        Logger::get().log("TEXTURE: Failed to load texture", true);
        return nullptr;
    }

    GLenum internalFormat = alpha ? GL_RGBA : GL_RGB;
    GLenum imageFormat = alpha ? GL_RGBA : GL_RGB;

    Texture* texture = new Texture(GL_TEXTURE_2D, width, height, nrChannels, internalFormat, imageFormat);
    texture->generate(data);
    stbi_image_free(data);

    return texture;
}