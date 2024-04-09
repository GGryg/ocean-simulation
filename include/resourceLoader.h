#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include <map>
#include <memory>
#include <string>

#include "shader.h"
#include "texture.h"

class ResourceLoader
{
public:
    ResourceLoader(ResourceLoader&) = delete;
    ResourceLoader& operator=(const ResourceLoader&) = delete;
    ResourceLoader(ResourceLoader&&) = delete;
    ResourceLoader& operator=(ResourceLoader&&) = delete;

    static ResourceLoader& get()
    {
        static ResourceLoader instance;
        return instance;
    }

    Shader loadShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    Shader loadShader(const std::string& computeShaderFile);

    Texture* loadTexture(const std::string& textureFile, bool alpha);

private:
    ResourceLoader() = default;
    Shader loadShaderFromFile(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    Shader loadComputeShaderFromFile(const std::string& computeShaderFile);
    Texture* loadTextureFromFile(const std::string& textureFile, bool alpha);  
};

#endif