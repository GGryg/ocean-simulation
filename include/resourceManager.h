#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include <map>
#include <string>

#include "shader.h"

class ResourceManager
{
public:
    ResourceManager(ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    static ResourceManager& get()
    {
        static ResourceManager instance;
        return instance;
    }

    Shader loadShader(const std::string& name, const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    Shader loadShader(const std::string& name, const std::string& computeShaderFile);
    Shader getShader(const std::string& name);

    void clear();
private:
    ResourceManager() = default;
    Shader loadShaderFromFile(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    Shader loadComputeShaderFromFile(const std::string& computeShaderFile);
    std::map<std::string, Shader> m_shaders;
};

#endif