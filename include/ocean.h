#ifndef OCEAN_H
#define OCEAN_H

#include <memory>

#include <glm/glm.hpp>
#include <GLAD/gl.h>

#include "shader.h"
#include "buffers.h"
#include "texture.h"

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
    float suppresorFactor;
    float displacementScale;
    int N;
};

struct PBR
{
    glm::vec3 sunDirection;
    glm::vec3 sunIrradiance;
    glm::vec3 scatterColor;
    glm::vec3 bubbleColor;
    float bubbleDensity;
    float wavePeakScatterStrength;
    float scatterStrength;
    float scatterShadowStrength;
    float heightWave;
    float roughness;
    float envLightStrength;
};

class Ocean
{
public:
    enum TextureVis
    {
        HIILDE0K,
        HIILDE0MINUSK,
        HIILDEKTDX,
        HIILDEKTDY,
        HIILDEKTDZ,
        DX,
        DY,
        DZ,
        NORMALMAP
    };

    Ocean(const SpectrumParams& params, const PBR& material);
    ~Ocean();

    void tilde_h0k();
    void tilde_hkt(float timeSpeed);

    void waving(float deltaTime, float timeSpeed); // update
    void draw(float deltaTime, glm::vec3 cameraPosition, glm::mat4 proj, glm::mat4 view, glm::mat4 model, int tiling);

    void setAmplitude(float amplitude);
    float amplitude() const;
    void setWindSpeed(float windSpeed);
    float windSpeed() const;
    void setwindDirection(const glm::vec2& windDirection);
    const glm::vec2& windDirection() const;
    void setLength(float length);
    float length() const;
    void setSuppresorFactor(float suppresorFactor);

    void setSunDirection(const glm::vec3& sunDirection);
    void setSunIrradiance(const glm::vec3& sunIrradiance);
    void setScatterColor(const glm::vec3& scatterColor);
    void setBubbleColor(const glm::vec3& bubbleColor);
    void setBubbleDensity(float bubbleDensity);
    void setWavePeakScatterStrength(float wavePeakScatterStrength);
    void setScatterStrength(float scatterStrength);
    void setScatterShadowStrength(float scatterShaderStrength);
    void setHeightWave(float heightWave);
    void setRoughness(float roughness);
    void setEnvLightStrength(float envLightStrength);

    void setChoppinessScale(const glm::vec2& choppinessScale);
    void setDisplacementScale(float displacementScale);

    GLuint texture(TextureVis textureVis) const;

    void prepareResources();

private:
    void generateMesh();
    void reverseIndices();
    GLuint reverseBits(GLuint n);
    void calculateTwiddleFactor();
    void butterflyOperation(std::unique_ptr<Texture>& input, std::unique_ptr<Texture>& output);
    void normalMap();

    PBR m_material;
    SpectrumParams m_spectrumParams;
    int m_N1{};
    int m_log_2_N;

    bool m_recalculateSpectrum;

    std::unique_ptr<Texture> m_noise0;
    std::unique_ptr<Texture> m_noise1;
    std::unique_ptr<Texture> m_noise2;
    std::unique_ptr<Texture> m_noise3;

    std::unique_ptr<Shader> m_tilde_h0k_shader;
    std::unique_ptr<Shader> m_tilde_hkt_shader;
    std::unique_ptr<Shader> m_twiddleFactors_shader;
    std::unique_ptr<Shader> m_butterflyOperation_shader;
    std::unique_ptr<Shader> m_inversion_shader;
    std::unique_ptr<Shader> m_normalMap_shader;
    std::unique_ptr<Shader> m_ocean_shader;

    std::unique_ptr<Texture> m_tilde_h0k;
    std::unique_ptr<Texture> m_tilde_h0minusk;
    std::unique_ptr<Texture> m_tilde_hkt_dx;
    std::unique_ptr<Texture> m_tilde_hkt_dy;
    std::unique_ptr<Texture> m_tilde_hkt_dz;

    std::unique_ptr<Texture> m_twiddleFactors;
    std::unique_ptr<Texture> m_pingPong;

    std::unique_ptr<Texture> m_dx;
    std::unique_ptr<Texture> m_dy;
    std::unique_ptr<Texture> m_dz;

    std::unique_ptr<Texture> m_normalMap;

    std::vector<OceanVertex> m_vertices;
    std::vector<GLuint> m_indices;
    std::unique_ptr<VArray> m_vao;
    std::unique_ptr<VBuffer> m_vbo;
    std::unique_ptr<EBuffer> m_ebo;
    std::unique_ptr<SSBuffer> m_ssbo;
};

#endif