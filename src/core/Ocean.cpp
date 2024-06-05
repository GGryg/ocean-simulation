#include "core/ocean.h"

#include "core/resourceLoader.h"
#include "core/constants.h"
#include "graphics/shaderException.h"

#include <cstddef>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glfft.hpp>
#include <glfft_gl_interface.hpp>

Ocean::Ocean(const SpectrumParams &params, const PBR &material, const OtherOptions &otherOptions)
	: m_material(material)
	, m_spectrumParams(params)
	, m_otherOptions(otherOptions)
	, m_recalculateSpectrum(true)
	, m_model(glm::mat4(1.0))
{
}

void Ocean::prepareResources()
{
	generateMesh();

	m_vao = std::make_unique<gfx::VArray>();
	m_vao->bind();
	m_vbo = std::make_unique<gfx::VBuffer>(m_vertices.data(), m_vertices.size() * sizeof(OceanVertex), GL_STATIC_DRAW);
	gfx::VBufferLayout elements;
	elements.emplace_back(3, GL_FLOAT, GL_FALSE, offsetof(OceanVertex, vertexPosition));
	elements.emplace_back(2, GL_FLOAT, GL_FALSE, offsetof(OceanVertex, texCoord));
	m_vao->addBuffer(m_vbo.get(), sizeof(OceanVertex), elements);
	m_ebo = std::make_unique<gfx::EBuffer>(m_indices.data(), m_indices.size(), GL_STATIC_DRAW);

	shaders["ocean"] = ResourceLoader::get().loadShader("shaders/ocean_vs.glsl", "shaders/ocean_fs.glsl");
	if (!shaders["ocean"]->isValid())
	{
		throw gfx::ShaderException("Failed to compile ocean shader");
	}

	textures["noise0"] = gfx::TexturePtr(ResourceLoader::get().loadTexture("resources/noise/noise0.png", true));
	textures["noise0"]->bind();
	textures["noise0"]->clampToEdge();
	textures["noise0"]->neareastFilter();
	textures["noise0"]->unbind();
	textures["noise1"] = gfx::TexturePtr(ResourceLoader::get().loadTexture("resources/noise/noise1.png", true));
	textures["noise1"]->bind();
	textures["noise1"]->clampToEdge();
	textures["noise1"]->neareastFilter();
	textures["noise1"]->unbind();
	textures["noise2"] = gfx::TexturePtr(ResourceLoader::get().loadTexture("resources/noise/noise2.png", true));
	textures["noise2"]->bind();
	textures["noise2"]->clampToEdge();
	textures["noise2"]->neareastFilter();
	textures["noise2"]->unbind();
	textures["noise3"] = gfx::TexturePtr(ResourceLoader::get().loadTexture("resources/noise/noise3.png", true));
	textures["noise3"]->bind();
	textures["noise3"]->clampToEdge();
	textures["noise3"]->neareastFilter();
	textures["noise3"]->unbind();


	shaders["tilde_h0k"] = ResourceLoader::get().loadShader("shaders/h_0_k_cs.glsl");
	if (!shaders["tilde_h0k"]->isValid())
	{
		throw gfx::ShaderException("Failed to compile tilde_h0k shader");
	}

	textures["tilde_h0k"] = std::make_unique<gfx::Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N,
	                                                       GL_RGBA32F, GL_RGBA);
	textures["tilde_h0k"]->bind();
	textures["tilde_h0k"]->allocateStorage(1);
	textures["tilde_h0k"]->clampToEdge();
	textures["tilde_h0k"]->neareastFilter();
	textures["tilde_h0k"]->unbind();
	textures["tilde_h0minusk"] = std::make_unique<gfx::Texture>(
		GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_RGBA32F, GL_RGBA);
	textures["tilde_h0minusk"]->bind();
	textures["tilde_h0minusk"]->allocateStorage(1);
	textures["tilde_h0minusk"]->clampToEdge();
	textures["tilde_h0minusk"]->neareastFilter();
	textures["tilde_h0minusk"]->unbind();

	shaders["tilde_hkt"] = ResourceLoader::get().loadShader("shaders/h_k_t_cs.glsl");
	if (!shaders["tilde_hkt"]->isValid())
	{
		throw gfx::ShaderException("Failed to compile tilde_hkt shader");
	}

	textures["tilde_hkt_dx"] = std::make_unique<gfx::Texture>(
		GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_RG32F, GL_RG);
	textures["tilde_hkt_dx"]->bind();
	textures["tilde_hkt_dx"]->allocateStorage(1);
	textures["tilde_hkt_dx"]->repeat();
	textures["tilde_hkt_dx"]->bilinearFilter();
	textures["tilde_hkt_dx"]->unbind();
	textures["tilde_hkt_dy"] = std::make_unique<gfx::Texture>(
		GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_RG32F, GL_RG);
	textures["tilde_hkt_dy"]->bind();
	textures["tilde_hkt_dy"]->allocateStorage(1);
	textures["tilde_hkt_dy"]->repeat();
	textures["tilde_hkt_dy"]->bilinearFilter();
	textures["tilde_hkt_dy"]->unbind();
	textures["tilde_hkt_dz"] = std::make_unique<gfx::Texture>(
		GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_RG32F, GL_RG);
	textures["tilde_hkt_dz"]->bind();
	textures["tilde_hkt_dz"]->allocateStorage(1);
	textures["tilde_hkt_dz"]->repeat();
	textures["tilde_hkt_dz"]->bilinearFilter();
	textures["tilde_hkt_dz"]->unbind();

	shaders["inversion"] = ResourceLoader::get().loadShader("shaders/inversion_cs.glsl");
	if (!shaders["inversion"]->isValid())
	{
		throw gfx::ShaderException("Failed to compile inversion shader");
	}

	textures["dx"] = std::make_unique<gfx::Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_R32F,
	                                                GL_RED);
	textures["dx"]->bind();
	textures["dx"]->allocateStorage(1);
	textures["dx"]->unbind();
	textures["dy"] = std::make_unique<gfx::Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_R32F,
	                                                GL_RED);
	textures["dy"]->bind();
	textures["dy"]->allocateStorage(1);
	textures["dy"]->unbind();
	textures["dz"] = std::make_unique<gfx::Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_R32F,
	                                                GL_RED);
	textures["dz"]->bind();
	textures["dz"]->allocateStorage(1);
	textures["dz"]->unbind();

	shaders["normalMap"] = ResourceLoader::get().loadShader("shaders/normal_map_cs.glsl");
	if (!shaders["normalMap"]->isValid())
	{
		throw gfx::ShaderException("Failed to compile normalMap shader");
	}

	textures["normalMap"] = std::make_unique<gfx::Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N,
	                                                       GL_RGBA32F, GL_RGBA);
	textures["normalMap"]->bind();
	textures["normalMap"]->allocateStorage(1);
	textures["normalMap"]->repeat();
	textures["normalMap"]->bilinearFilter();
	textures["normalMap"]->unbind();
}


void Ocean::generateMesh()
{
	m_vertices.reserve(constants::gridSizeP1 * constants::gridSizeP1);
	m_indices.reserve(constants::gridSizeP1 * constants::gridSizeP1 * 3);
	constexpr int half = constants::gridSize / 2;

	// vertices
	for (int z = -half; z <= half; ++z)
	{
		for (int x = -half; x <= half; ++x)
		{
			float texU = (static_cast<float>(x + half) / static_cast<float>(constants::gridSize)) *
			             constants::gridUVSize;
			float texV = (static_cast<float>(z + half) / static_cast<float>(constants::gridSize)) *
			             constants::gridUVSize;

			m_vertices.emplace_back(glm::vec3(static_cast<float>(x), 0.0f, static_cast<float>(z)),
			                        glm::vec2(texU, texV));
		}
	}

	// indices
	for (int i = 0; i < constants::gridSize; i++)
	{
		for (int j = 0; j < constants::gridSize; j++)
		{
			std::size_t idx0 = constants::gridSizeP1 * i + j;
			std::size_t idx1 = constants::gridSizeP1 * (i + 1) + j;
			std::size_t idx2 = idx0 + 1;
			std::size_t idx3 = idx2;
			std::size_t idx4 = idx1;
			std::size_t idx5 = idx1 + 1;

			m_indices.push_back(idx0);
			m_indices.push_back(idx1);
			m_indices.push_back(idx2);
			m_indices.push_back(idx3);
			m_indices.push_back(idx4);
			m_indices.push_back(idx5);
		}
	}
}

void Ocean::initialSpectrum()
{
	shaders["tilde_h0k"]->use();
	textures["noise0"]->bindActive(2);
	shaders["tilde_h0k"]->setInt("u_noise0", 2);
	textures["noise1"]->bindActive(3);
	shaders["tilde_h0k"]->setInt("u_noise1", 3);
	textures["noise2"]->bindActive(4);
	shaders["tilde_h0k"]->setInt("u_noise2", 4);
	textures["noise3"]->bindActive(5);
	shaders["tilde_h0k"]->setInt("u_noise3", 5);

	shaders["tilde_h0k"]->setFloat("u_amplitude", m_spectrumParams.amplitude);
	shaders["tilde_h0k"]->setFloat("u_windSpeed", m_spectrumParams.windSpeed);
	shaders["tilde_h0k"]->setVec2("u_WindDirection", m_spectrumParams.windDirection);
	shaders["tilde_h0k"]->setInt("u_N", m_spectrumParams.N);
	shaders["tilde_h0k"]->setFloat("u_L", m_spectrumParams.length);
	shaders["tilde_h0k"]->setFloat("u_suppressorFactor", m_spectrumParams.suppressorFactor);

	textures["tilde_h0k"]->bindImage(0, 0, 0, GL_WRITE_ONLY);
	textures["tilde_h0minusk"]->bindImage(1, 0, 0, GL_WRITE_ONLY);

	glDispatchCompute(8, 8, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glFinish();
}

void Ocean::spectrumInTime(float timeSpeed)
{
	shaders["tilde_hkt"]->use();

	textures["tilde_h0k"]->bindImage(0, 0, 0, GL_READ_ONLY);
	textures["tilde_h0minusk"]->bindImage(1, 0, 0, GL_READ_ONLY);
	textures["tilde_hkt_dx"]->bindImage(2, 0, 0, GL_WRITE_ONLY);
	textures["tilde_hkt_dy"]->bindImage(3, 0, 0, GL_WRITE_ONLY);
	textures["tilde_hkt_dz"]->bindImage(4, 0, 0, GL_WRITE_ONLY);

	shaders["tilde_hkt"]->setInt("u_N", m_spectrumParams.N);
	shaders["tilde_hkt"]->setFloat("u_L", m_spectrumParams.length);
	shaders["tilde_hkt"]->setFloat("u_t", static_cast<float>(glfwGetTime()) * timeSpeed);
	shaders["tilde_hkt"]->setFloat("u_amplitude", m_spectrumParams.amplitude);

	glDispatchCompute(8, 8, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glFinish();
}

void Ocean::inverse()
{
	shaders["inversion"]->use();

	shaders["inversion"]->setInt("u_N", m_spectrumParams.N);

	textures["dx"]->bindImage(0, 0, 0, GL_READ_WRITE);
	textures["dy"]->bindImage(1, 0, 0, GL_READ_WRITE);
	textures["dz"]->bindImage(2, 0, 0, GL_READ_WRITE);

	glDispatchCompute(8, 8, 1);
	glFinish();
}

void Ocean::normalMap()
{
	shaders["normalMap"]->use();
	textures["normalMap"]->bindImage(0, 0, 0, GL_WRITE_ONLY);

	shaders["normalMap"]->setInt("u_dy", 1);
	textures["dy"]->bindActive(1);

	shaders["normalMap"]->setInt("u_N", m_spectrumParams.N);

	glDispatchCompute(8, 8, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glFinish();
}

void Ocean::fft(const std::string &inputTextureKey, const std::string &outputTextureKey)
{
	GLFFT::FFTOptions options;
	options.type.fp16 = false;
	options.type.output_fp16 = false;
	options.type.input_fp16 = false;
	options.type.normalize = true;
	GLFFT::GLContext context;

	GLFFT::FFT fft(&context, m_spectrumParams.N, m_spectrumParams.N, GLFFT::ComplexToReal, GLFFT::Inverse, GLFFT::Image,
	               GLFFT::ImageReal, std::make_shared<GLFFT::ProgramCache>(), options);

	GLFFT::GLTexture adaptor_input(textures[inputTextureKey]->id());
	GLFFT::GLTexture adaptor_output(textures[outputTextureKey]->id());

	GLFFT::CommandBuffer *cmd = context.request_command_buffer();
	fft.process(cmd, &adaptor_output, &adaptor_input);
	cmd->barrier();
	context.submit_command_buffer(cmd);
	context.wait_idle();
}

void Ocean::waving()
{
	if (m_recalculateSpectrum)
	{
		initialSpectrum();
		m_recalculateSpectrum = false;
	}
	spectrumInTime(m_otherOptions.speed);

	fft("tilde_hkt_dy", "dy");
	fft("tilde_hkt_dx", "dx");
	fft("tilde_hkt_dz", "dz");
	inverse();

	normalMap();
}

void Ocean::draw(const glm::vec3 &cameraPosition, const glm::mat4 &proj, const glm::mat4 &view, bool wireframeMode)
{
	shaders["ocean"]->use();

	m_vao->bind();

	shaders["ocean"]->setMat4("u_proj", proj);
	shaders["ocean"]->setMat4("u_view", view);
	shaders["ocean"]->setMat4("u_model", m_model);
	shaders["ocean"]->setInt("u_dx", 0);
	textures["dx"]->bindActive(0);
	shaders["ocean"]->setInt("u_dy", 1);
	textures["dy"]->bindActive(1);
	shaders["ocean"]->setInt("u_dz", 2);
	textures["dz"]->bindActive(2);
	shaders["ocean"]->setInt("u_normalMap", 3);
	textures["normalMap"]->bindActive(3);
	shaders["ocean"]->setInt("skybox", 4);
	shaders["ocean"]->setFloat("u_displacement", m_spectrumParams.displacementScale);
	shaders["ocean"]->setVec2("u_choppiness", m_spectrumParams.choppinessScale);

	// lightining/shadows
	shaders["ocean"]->setVec3("u_viewPosition", cameraPosition);
	shaders["ocean"]->setBool("u_wireframeMode", wireframeMode);

	shaders["ocean"]->setFloat("u_roughness", m_material.roughness);
	shaders["ocean"]->setVec3("u_sunDirection", m_material.sunDirection);
	shaders["ocean"]->setVec3("u_sunColor", m_material.sunColor);
	shaders["ocean"]->setVec3("u_scatterColor", m_material.scatterColor);
	shaders["ocean"]->setVec3("u_bubbleColor", m_material.bubbleColor);
	shaders["ocean"]->setFloat("u_bubbleDensity", m_material.bubbleDensity);
	shaders["ocean"]->setFloat("u_wavePeakScatterStrength", m_material.wavePeakScatterStrength);
	shaders["ocean"]->setFloat("u_scatterStrength", m_material.scatterStrength);
	shaders["ocean"]->setFloat("u_scatterShadowStrength", m_material.scatterShadowStrength);
	shaders["ocean"]->setFloat("u_waveHeight", m_material.heightWave);
	shaders["ocean"]->setFloat("u_envLightStrength", m_material.envLightStrength);

	for (int j = 0; j < m_otherOptions.tiling; ++j)
	{
		for (int i = 0; i < m_otherOptions.tiling; ++i)
		{
			m_model = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
			m_model = glm::translate(m_model, glm::vec3(1024 * i, 0, 1024 * -j));
			shaders["ocean"]->setMat4("u_model", m_model);
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_ebo->count()), GL_UNSIGNED_INT, nullptr);
		}
	}
}

void Ocean::setAmplitude(float amplitude)
{
	m_spectrumParams.amplitude = amplitude;
	m_recalculateSpectrum = true;
}

float Ocean::amplitude() const
{
	return m_spectrumParams.amplitude;
}

void Ocean::setWindSpeed(float windSpeed)
{
	m_spectrumParams.windSpeed = windSpeed;
	m_recalculateSpectrum = true;
}

float Ocean::windSpeed() const
{
	return m_spectrumParams.windSpeed;
}

void Ocean::setWindDirection(const glm::vec2 &windDirection)
{
	m_spectrumParams.windDirection = windDirection;
	m_recalculateSpectrum = true;
}

const glm::vec2 &Ocean::windDirection() const
{
	return m_spectrumParams.windDirection;
}

void Ocean::setLength(float length)
{
	m_spectrumParams.length = length;
	m_recalculateSpectrum = true;
}

float Ocean::length() const
{
	return m_spectrumParams.length;
}

void Ocean::setSuppressorFactor(float suppressorFactor)
{
	m_spectrumParams.suppressorFactor = suppressorFactor;
	m_recalculateSpectrum = true;
}

void Ocean::setSunDirection(const glm::vec3 &sunDirection)
{
	m_material.sunDirection = sunDirection;
}

void Ocean::setSunColor(const glm::vec3 &sunColor)
{
	m_material.sunColor = sunColor;
}

void Ocean::setScatterColor(const glm::vec3 &scatterColor)
{
	m_material.scatterColor = scatterColor;
}

void Ocean::setBubbleColor(const glm::vec3 &bubbleColor)
{
	m_material.bubbleColor = bubbleColor;
}

void Ocean::setBubbleDensity(float bubbleDensity)
{
	m_material.bubbleDensity = bubbleDensity;
}

void Ocean::setWavePeakScatterStrength(float wavePeakScatterStrength)
{
	m_material.wavePeakScatterStrength = wavePeakScatterStrength;
}

void Ocean::setScatterStrength(float scatterStrength)
{
	m_material.scatterStrength = scatterStrength;
}

void Ocean::setScatterShadowStrength(float scatterShaderStrength)
{
	m_material.scatterShadowStrength = scatterShaderStrength;
}

void Ocean::setHeightWave(float heightWave)
{
	m_material.heightWave = heightWave;
}

void Ocean::setRoughness(float roughness)
{
	m_material.roughness = roughness;
}

void Ocean::setEnvLightStrength(float envLightStrength)
{
	m_material.envLightStrength = envLightStrength;
}

void Ocean::setChoppinessScale(const glm::vec2 &choppinessScale)
{
	m_spectrumParams.choppinessScale = choppinessScale;
}

void Ocean::setDisplacementScale(float displacementScale)
{
	m_spectrumParams.displacementScale = displacementScale;
}

void Ocean::setTiling(int tiling)
{
	m_otherOptions.tiling = tiling;
}

void Ocean::setSpeed(float speed)
{
	m_otherOptions.speed = speed;
}


GLuint Ocean::texture(TextureVis textureVis) const
{
	using enum TextureVis;
	switch (textureVis)
	{
		case hildeh0k:			return textures.at("tilde_h0k")->id();
		case hildehminusk0k:	return textures.at("tilde_h0minusk")->id();
		case hildehktDX:		return textures.at("tilde_hkt_dx")->id();
		case hildehktDY:		return textures.at("tilde_hkt_dy")->id();
		case hildehktDZ:		return textures.at("tilde_hkt_dz")->id();
		case dx:				return textures.at("dx")->id();
		case dy:                return textures.at("dy")->id();
		case dz:                return textures.at("dz")->id();
		case normalMap:         return textures.at("normalMap")->id();
	}
	return 0;
}
