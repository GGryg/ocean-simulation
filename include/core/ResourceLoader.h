#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include <memory>
#include <string>
#include <vector>

#include "graphics/Shader.h"
#include "graphics/Texture.h"

class ResourceLoader
{
public:
	ResourceLoader(ResourceLoader &) = delete;

	ResourceLoader &operator=(const ResourceLoader &) = delete;

	ResourceLoader(ResourceLoader &&) = delete;

	ResourceLoader &operator=(ResourceLoader &&) = delete;

	static ResourceLoader &get()
	{
		static ResourceLoader instance;
		return instance;
	}

	gfx::ShaderPtr loadShader(const std::string &vertexShaderFile, const std::string &fragmentShaderFile);

	gfx::ShaderPtr loadShader(const std::string &computeShaderFile);

	gfx::TexturePtr loadTexture(const std::string &textureFile, bool alpha);

	gfx::TexturePtr loadCubemap(const std::vector<std::string> &facesPath);

private:
	ResourceLoader() = default;

	std::string loadShaderFromFile(const std::string &shaderPath);
};

#endif //RESOURCE_LOADER_H
