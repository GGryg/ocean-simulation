#ifndef OCEAN_H
#define OCEAN_H

#include <memory>

#include <glm/glm.hpp>
#include <unordered_map>

#include "graphics/Shader.h"
#include "graphics/VBuffer.h"
#include "graphics/VArray.h"
#include "graphics/EBuffer.h"
#include "graphics/Texture.h"

struct OceanVertex
{
	glm::vec3 vertexPosition;
	glm::vec2 texCoord;
};

struct SpectrumParams
{
	glm::vec2 windDirection;
	glm::vec2 choppinessScale;
	float amplitude;
	float windSpeed;
	float length;
	float suppressorFactor;
	float displacementScale;
	int N;
};

struct PBR
{
	glm::vec3 sunDirection;
	glm::vec3 sunColor;
	glm::vec3 scatterColor;
	glm::vec3 bubbleColor;
	float bubbleDensity;
	float wavePeakScatterStrength;
	float scatterStrength;
	float scatterShadowStrength;
	float waveHeight;
	float roughness;
	float envLightStrength;
};

struct OtherOptions
{
	float speed;
	int tiling;
};

class Ocean
{
public:
	enum class TextureVis
	{
		hildeh0k,
		hildehminusk0k,
		hildehktDX,
		hildehktDY,
		hildehktDZ,
		dx,
		dy,
		dz,
		normalMap
	};

	Ocean(const SpectrumParams &params, const PBR &material, const OtherOptions &otherOptions);

	~Ocean() = default;

	void prepareResources();

	void waving(); // update
	void draw(const glm::vec3 &cameraPosition, const glm::mat4 &proj, const glm::mat4 &view, bool wireframeMode);

	void setAmplitude(float amplitude);

	void setWindSpeed(float windSpeed);

	void setWindDirection(const glm::vec2 &windDirection);

	void setLength(float length);

	void setSuppressorFactor(float suppressorFactor);

	void setSunDirection(const glm::vec3 &sunDirection);

	void setSunColor(const glm::vec3 &sunColor);

	void setScatterColor(const glm::vec3 &scatterColor);

	void setBubbleColor(const glm::vec3 &bubbleColor);

	void setBubbleDensity(float bubbleDensity);

	void setWavePeakScatterStrength(float wavePeakScatterStrength);

	void setScatterStrength(float scatterStrength);

	void setScatterShadowStrength(float scatterShaderStrength);

	void setWaveHeight(float waveHeight);

	void setRoughness(float roughness);

	void setEnvLightStrength(float envLightStrength);

	void setChoppinessScale(const glm::vec2 &choppinessScale);

	void setDisplacementScale(float displacementScale);

	void setTiling(int tiling);

	void setSpeed(float speed);

	GLuint texture(TextureVis textureVis) const;

private:
	void generateMesh();

	void initialSpectrum();

	void spectrumInTime(float timeSpeed);

	void inverse();

	void normalMap();

	void fft(const std::string &inputTextureKey, const std::string &outputTextureKey);

	PBR m_material;
	SpectrumParams m_spectrumParams;
	OtherOptions m_otherOptions;

	bool m_recalculateSpectrum;

	std::unordered_map<std::string, gfx::TexturePtr> textures;
	std::unordered_map<std::string, gfx::ShaderPtr> shaders;

	std::vector<OceanVertex> m_vertices;
	std::vector<GLuint> m_indices;
	gfx::VArrayPtr m_vao;
	gfx::VBufferPtr m_vbo;
	gfx::EBufferPtr m_ebo;

	glm::mat4 m_model;
};

#endif //OCEAN_H
