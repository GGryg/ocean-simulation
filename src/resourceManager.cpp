#include "resourceManager.h"
#include "logger.h"
#include <fstream>
#include <sstream>

Shader ResourceManager::loadShader(const std::string& name, const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{  
    m_shaders[name] = loadShaderFromFile(vertexShaderFile, fragmentShaderFile);
    Logger::get().log("Loaded  shader:" + name + " from files: " + vertexShaderFile + ", " + fragmentShaderFile);
    return m_shaders[name];
}

Shader ResourceManager::loadShader(const std::string& name, const std::string& computeShaderFile)
{
    m_shaders[name] = loadComputeShaderFromFile(computeShaderFile);
    Logger::get().log("Loaded  shader:" + name + "from file: " + computeShaderFile);
    return m_shaders[name];
}

Shader ResourceManager::getShader(const std::string& name)
{
    return m_shaders[name];
}

void ResourceManager::clear()
{
    Logger::get().log("Clearing loaded shaders");
    m_shaders.clear();
}

Shader ResourceManager::loadShaderFromFile(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
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

Shader ResourceManager::loadComputeShaderFromFile(const std::string& computeShaderFile)
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