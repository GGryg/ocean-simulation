#include "core/ResourceLoader.h"
#include "logger/Logger.h"
#include "graphics/TextureException.h"
#include <fstream>
#include <sstream>
#include <exception>

#include <glad/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <graphics/ShaderException.h>

gfx::ShaderPtr ResourceLoader::loadShader(const std::string &vertexShaderFile, const std::string &fragmentShaderFile)
{
	Logger::get().log("Loading vertex and fragment shader from files: " + vertexShaderFile + ", " + fragmentShaderFile);
	std::string vertexShader = loadShaderFromFile(vertexShaderFile);
	std::string fragmentShader = loadShaderFromFile(fragmentShaderFile);

	return std::make_unique<gfx::Shader>(vertexShader, fragmentShader);
}

gfx::ShaderPtr ResourceLoader::loadShader(const std::string &computeShaderFile)
{
	Logger::get().log("Loading compute shader from file: " + computeShaderFile);
	std::string computeShader = loadShaderFromFile(computeShaderFile);

	return std::make_unique<gfx::Shader>(computeShader);
}

std::string ResourceLoader::loadShaderFromFile(const std::string &shaderPath)
{
	std::string shaderSource;
	std::ifstream shaderFile;

	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;

		shaderStream << shaderFile.rdbuf();

		shaderFile.close();

		shaderSource = shaderStream.str();
	}
	catch (std::exception &ex)
	{
		throw gfx::ShaderException("Failed to read shader file: " + shaderPath);
	}

	return shaderSource;
}

gfx::TexturePtr ResourceLoader::loadTexture(const std::string &textureFile, bool alpha)
{
	Logger::get().log("Loading texture from file: " + textureFile);

	GLint width;
	GLint height;
	GLint nrChannels;
	unsigned char *data = stbi_load(textureFile.c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		throw gfx::TextureException("Failed to load texture");
	}

	GLenum internalFormat = alpha ? GL_RGBA : GL_RGB;
	GLenum imageFormat = alpha ? GL_RGBA : GL_RGB;

	gfx::TexturePtr texture = std::make_unique<gfx::Texture>(
		GL_TEXTURE_2D, width, height, nrChannels, internalFormat, imageFormat);
	texture->generate(data);
	stbi_image_free(data);

	return std::move(texture);
}

gfx::TexturePtr ResourceLoader::loadCubemap(const std::vector<std::string> &facesPath)
{
	Logger::get().log("Loading cubemap texture");

	GLint width;
	GLint height;
	GLint nrChannels;
	gfx::TexturePtr texture = std::make_unique<gfx::Texture>(GL_TEXTURE_CUBE_MAP, width, height);
	texture->bind();
	for (GLuint i = 0; i < facesPath.size(); ++i)
	{
		unsigned char *data = stbi_load(facesPath[i].c_str(), &width, &height, &nrChannels, 0);
		texture->setWidth(width);
		texture->setHeight(height);
		if (!data)
		{
			throw gfx::TextureException("Failed to load cubemap texture: " + facesPath[i]);
		}
		texture->texImageCubemap(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, data);

		stbi_image_free(data);
	}
	texture->clampToEdgeCube();
	texture->trilinearFilter();

	return std::move(texture);
}
