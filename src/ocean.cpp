#include "ocean.h"
#include "resourceLoader.h"
#include "constants.h"
#include "shaderException.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glfft.hpp>
#include <glfft_gl_interface.hpp>

Ocean::Ocean(const SpectrumParams& params, const PBR& material)
    : m_spectrumParams(params)
    , m_material(material)
    , m_recalculateSpectrum(true)
{
    m_N1 = m_spectrumParams.N + 1;
    m_log_2_N = static_cast<int>(std::log(m_spectrumParams.N) / std::log(2));
}

Ocean::~Ocean()
{

}

void Ocean::prepareResources()
{
    generateMesh();

    m_vao = std::make_unique<VArray>();
    m_vao->bind();
    m_vbo = std::make_unique<VBuffer>(m_vertices.data(), m_vertices.size() * sizeof(OceanVertex), GL_STATIC_DRAW);
    VBufferLayout elements;
    elements.emplace_back(3, GL_FLOAT, GL_FALSE, offsetof(OceanVertex, vertexPosition));
    elements.emplace_back(2, GL_FLOAT, GL_FALSE, offsetof(OceanVertex, texCoord));
    m_vao->addBuffer(m_vbo.get(), sizeof(OceanVertex), elements);
    m_ebo = std::make_unique<EBuffer>(m_indices.data(), m_indices.size(), GL_STATIC_DRAW);
    // initialize ssbo
    reverseIndices();

    m_ocean_shader = ResourceLoader::get().loadShader("shaders/ocean_vs.glsl", "shaders/ocean_fs.glsl");
    if(!m_ocean_shader->isValid())
    {
        throw ShaderException("Failed to compile ocean shader");
    }

    m_noise0 = std::unique_ptr<Texture>(ResourceLoader::get().loadTexture("resources/noise/noise0.png", true));
    m_noise0->bind();
    m_noise0->clampToEdge();
    m_noise0->neareastFilter();
    m_noise0->unbind();
    m_noise1 = std::unique_ptr<Texture>(ResourceLoader::get().loadTexture("resources/noise/noise1.png", true));
    m_noise1->bind();
    m_noise1->clampToEdge();
    m_noise1->neareastFilter();
    m_noise1->unbind();
    m_noise2 = std::unique_ptr<Texture>(ResourceLoader::get().loadTexture("resources/noise/noise2.png", true));
    m_noise2->bind();
    m_noise2->clampToEdge();
    m_noise2->neareastFilter();
    m_noise2->unbind();
    m_noise3 = std::unique_ptr<Texture>(ResourceLoader::get().loadTexture("resources/noise/noise3.png", true));
    m_noise3->bind();
    m_noise3->clampToEdge();
    m_noise3->neareastFilter();
    m_noise3->unbind();


    m_tilde_h0k_shader = ResourceLoader::get().loadShader("shaders/h_0_k_cs.glsl");
    if(!m_tilde_h0k_shader->isValid())
    {
        throw ShaderException("Failed to compile tilde_h0k shader");
    }

    m_tilde_h0k = std::make_unique<Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_RGBA32F, GL_RGBA);
    m_tilde_h0k->bind();
    m_tilde_h0k->allocateStorage(1);
    m_tilde_h0k->clampToEdge();
    m_tilde_h0k->neareastFilter();
    m_tilde_h0k->unbind();
    m_tilde_h0minusk = std::make_unique<Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_RGBA32F, GL_RGBA);
    m_tilde_h0minusk->bind();
    m_tilde_h0minusk->allocateStorage(1);
    m_tilde_h0minusk->clampToEdge();
    m_tilde_h0minusk->neareastFilter();
    m_tilde_h0minusk->unbind();

    m_tilde_hkt_shader = ResourceLoader::get().loadShader("shaders/h_k_t_cs.glsl");
    if(!m_tilde_hkt_shader->isValid())
    {
        throw ShaderException("Failed to compile tilde_hkt shader");
    }

    m_tilde_hkt_dx = std::make_unique<Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_RG32F, GL_RG);
    m_tilde_hkt_dx->bind();
    m_tilde_hkt_dx->allocateStorage(1);
    m_tilde_hkt_dx->repeat();
    m_tilde_hkt_dx->bilinearFilter();
    m_tilde_hkt_dx->unbind();
    m_tilde_hkt_dy = std::make_unique<Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_RG32F, GL_RG);
    m_tilde_hkt_dy->bind();
    m_tilde_hkt_dy->allocateStorage(1);
    m_tilde_hkt_dy->repeat();
    m_tilde_hkt_dy->bilinearFilter();
    m_tilde_hkt_dy->unbind();
    m_tilde_hkt_dz = std::make_unique<Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_RG32F, GL_RG);
    m_tilde_hkt_dz->bind();
    m_tilde_hkt_dz->allocateStorage(1);
    m_tilde_hkt_dz->repeat();
    m_tilde_hkt_dz->bilinearFilter();
    m_tilde_hkt_dz->unbind();

    m_twiddleFactors_shader = ResourceLoader::get().loadShader("shaders/twiddle_factors_cs.glsl");
    if(!m_twiddleFactors_shader->isValid())
    {
        throw ShaderException("Failed to compile twiddleFactors shader");
    }

    m_twiddleFactors = std::make_unique<Texture>(GL_TEXTURE_2D, m_log_2_N, m_spectrumParams.N, GL_RGBA32F, GL_RGBA);
    m_twiddleFactors->bind();
    m_twiddleFactors->allocateStorage(1);
    m_twiddleFactors->clampToEdge();
    m_twiddleFactors->neareastFilter();
    m_twiddleFactors->unbind();

    m_butterflyOperation_shader = ResourceLoader::get().loadShader("shaders/butterfly_cs.glsl");
    if(!m_butterflyOperation_shader->isValid())
    {
        throw ShaderException("Failed to compile butterflyOperation shader");
    }
    m_inversion_shader = ResourceLoader::get().loadShader("shaders/inversion_cs.glsl");
    if(!m_inversion_shader->isValid())
    {
        throw ShaderException("Failed to compile inversion shader");
    }

    m_pingPong = std::make_unique<Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_R32F, GL_RED);
    m_pingPong->bind();
    m_pingPong->allocateStorage(1);
    m_pingPong->unbind();

    m_dx = std::make_unique<Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_R32F, GL_RED);
    m_dx->bind();
    m_dx->allocateStorage(1);
    m_dx->unbind();
    m_dy = std::make_unique<Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_R32F, GL_RED);
    m_dy->bind();
    m_dy->allocateStorage(1);
    m_dy->unbind();
    m_dz = std::make_unique<Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_R32F, GL_RED);
    m_dz->bind();
    m_dz->allocateStorage(1);
    m_dz->unbind();
    testx = std::make_unique<Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_R32F, GL_RED);
    testx->bind();
    testx->allocateStorage(1);
    testx->unbind();
    testy = std::make_unique<Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_R32F, GL_RED);
    testy->bind();
    testy->allocateStorage(1);
    testy->unbind();
    testz = std::make_unique<Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_R32F, GL_RED);
    testz->bind();
    testz->allocateStorage(1);
    testz->unbind();

    m_normalMap_shader = ResourceLoader::get().loadShader("shaders/normal_map_cs.glsl");
    if(!m_normalMap_shader->isValid())
    {
        throw ShaderException("Failed to compile normalMap shader");
    }

    m_normalMap = std::make_unique<Texture>(GL_TEXTURE_2D, m_spectrumParams.N, m_spectrumParams.N, GL_RGBA32F, GL_RGBA);
    m_normalMap->bind();
    m_normalMap->allocateStorage(1);
    m_normalMap->repeat();
    m_normalMap->bilinearFilter();
    m_normalMap->unbind();

    calculateTwiddleFactor();
}


void Ocean::generateMesh()
{
    m_vertices.reserve(constants::gridSizeP1 * constants::gridSizeP1);
    m_indices.reserve(constants::gridSizeP1 * constants::gridSizeP1 * 3);
    const int half = constants::gridSize / 2;

    // vertices
    for(int z = -half; z <= half; ++z)
    {
        for(int x = -half; x <= half; ++x)
        {
            float texU = ((x + half) / static_cast<float>(constants::gridSize)) * constants::gridUVSize;
            float texV = ((z + half) / static_cast<float>(constants::gridSize)) * constants::gridUVSize;

            m_vertices.emplace_back(glm::vec3(static_cast<float>(x), 0.0f, static_cast<float>(z)), glm::vec2(texU, texV));
        }
    }

    // indices
    for(int i = 0; i < constants::gridSize; i++)
    {
        for(int j = 0; j < constants::gridSize; j++)
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

void Ocean::tilde_h0k()
{
    m_tilde_h0k_shader->use();
    m_noise0->bindActive(2);
    m_tilde_h0k_shader->setInt("noise0", 2);
    m_noise1->bindActive(3);
    m_tilde_h0k_shader->setInt("noise1", 3);
    m_noise2->bindActive(4);
    m_tilde_h0k_shader->setInt("noise2", 4);
    m_noise3->bindActive(5);
    m_tilde_h0k_shader->setInt("noise3", 5);

    m_tilde_h0k_shader->setFloat("u_amplitude", m_spectrumParams.amplitude);
    m_tilde_h0k_shader->setFloat("u_windSpeed", m_spectrumParams.windSpeed);
    m_tilde_h0k_shader->setVec2("u_WindDirection", m_spectrumParams.windDirection);
    m_tilde_h0k_shader->setInt("u_N", m_spectrumParams.N);
    m_tilde_h0k_shader->setFloat("u_L", m_spectrumParams.length);
    m_tilde_h0k_shader->setFloat("u_suppresorFactor", m_spectrumParams.suppresorFactor);

    m_tilde_h0k->bindImage(0, 0, 0, GL_WRITE_ONLY);
    m_tilde_h0minusk->bindImage(1, 0, 0, GL_WRITE_ONLY);

    glDispatchCompute(32, 32, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glFinish();
}

void Ocean::tilde_hkt(float timeSpeed)
{
    m_tilde_hkt_shader->use();

    m_tilde_h0k->bindImage(0, 0, 0, GL_READ_ONLY);
    m_tilde_h0minusk->bindImage(1, 0, 0, GL_READ_ONLY);
    m_tilde_hkt_dx->bindImage(2, 0, 0, GL_WRITE_ONLY);
    m_tilde_hkt_dy->bindImage(3, 0, 0, GL_WRITE_ONLY);
    m_tilde_hkt_dz->bindImage(4, 0, 0, GL_WRITE_ONLY);

    m_tilde_hkt_shader->setInt("u_N", m_spectrumParams.N);
    m_tilde_hkt_shader->setFloat("u_L", m_spectrumParams.length);
    m_tilde_hkt_shader->setFloat("u_t", static_cast<float>(glfwGetTime()) * timeSpeed);
    m_tilde_hkt_shader->setFloat("u_amplitude", m_spectrumParams.amplitude);

    glDispatchCompute(8, 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

GLuint Ocean::reverseBits(GLuint n)
{
    GLuint result = 0;
    int numBits = static_cast<int>(std::log(m_spectrumParams.N) / std::log(2));
    for(int i = 0; i < numBits; ++i)
    {
        result |= (n & 1) << (numBits - 1 - i);
        n >>= 1;
    }
    return result;
}

void Ocean::reverseIndices()
{
    std::vector<GLuint> indices;

    for(int i = 0; i < m_spectrumParams.N; ++i)
    {
        indices.push_back(reverseBits(i));
    }
    m_ssbo = std::make_unique<SSBuffer>(indices.data(), indices.size() * sizeof(GLuint), GL_STATIC_COPY);
}

void Ocean::calculateTwiddleFactor()
{
    m_twiddleFactors_shader->use();

    m_twiddleFactors->bindImage(0, 0, 0, GL_WRITE_ONLY);
    m_ssbo->bindBase(1);

    m_twiddleFactors_shader->setInt("u_N", m_spectrumParams.N);

    glDispatchCompute(m_log_2_N, 8, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glFinish();

}

void Ocean::butterflyOperation(std::unique_ptr<Texture>& input, std::unique_ptr<Texture>& output)
{
    m_butterflyOperation_shader->use();
    m_twiddleFactors->bindImage(0, 0, 0, GL_READ_ONLY);
    input->bindImage(1, 0, 0, GL_READ_WRITE);
    m_pingPong->bindImage(2, 0, 0, GL_READ_WRITE);
    glDispatchCompute(8, 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);//
    int pingpong = 0;
    /*
    for(int i = 0; i < m_log_2_N; i++)
    {
        m_butterflyOperation_shader->setInt("u_pingpong", pingpong);
        m_butterflyOperation_shader->setInt("u_isVertical", 0);
        m_butterflyOperation_shader->setInt("u_stage", i);

        glDispatchCompute(8, 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        pingpong ^= 1;
    }

    for(int i = 0; i < m_log_2_N; i++)
    {
        m_butterflyOperation_shader->setInt("u_pingpong", pingpong);
        m_butterflyOperation_shader->setInt("u_isVertical", 1);
        m_butterflyOperation_shader->setInt("u_stage", i);

        glDispatchCompute(8, 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        pingpong ^= 1;
    }
    */
    m_inversion_shader->use();

    m_inversion_shader->setInt("u_pingpong", pingpong);
    m_inversion_shader->setInt("u_N", m_spectrumParams.N);

    output->bindImage(0, 0, 0, GL_WRITE_ONLY);
    input->bindImage(1, 0, 0, GL_READ_ONLY);
    m_pingPong->bindImage(2, 0, 0, GL_READ_ONLY);

    glDispatchCompute(8, 8, 1);
    glFinish();
}

void Ocean::normalMap()
{
    m_normalMap_shader->use();
    m_normalMap->bindImage(0, 0, 0, GL_WRITE_ONLY);

    m_normalMap_shader->setInt("u_dy", 1);
    m_dy->bindActive(1);

    m_normalMap_shader->setInt("u_N", m_spectrumParams.N);

    glDispatchCompute(8, 8, 1);
    glFinish();
}

void Ocean::testFFT(std::unique_ptr<Texture>& input, std::unique_ptr<Texture>& output)
{
    GLFFT::FFTOptions options;
    options.type.fp16 = false;
    options.type.output_fp16 = false;
    options.type.input_fp16 = false;
    options.type.normalize = true;
    GLFFT::GLContext context;

    // doesn't work
    GLFFT::FFT fft(&context, m_spectrumParams.N, m_spectrumParams.N, GLFFT::ComplexToReal, GLFFT::Inverse, GLFFT::Image, GLFFT::ImageReal, std::make_shared<GLFFT::ProgramCache>(), options);

    GLFFT::GLTexture adaptor_input(input->id());
    GLFFT::GLTexture adaptor_output(output->id());

    GLFFT::CommandBuffer* cmd = context.request_command_buffer();
    fft.process(cmd, &adaptor_output, &adaptor_input);
    cmd->barrier();
    context.submit_command_buffer(cmd);
    context.wait_idle();

}

void Ocean::waving(float deltaTime, float timeSpeed)
{
    if(m_recalculateSpectrum)
    {
        tilde_h0k();
        m_recalculateSpectrum = false;
    }

    tilde_hkt(timeSpeed);
    testFFT(m_tilde_hkt_dy, testy);
    testFFT(m_tilde_hkt_dx, testx);
    testFFT(m_tilde_hkt_dz, testz);
    butterflyOperation(testx, m_dx);
    butterflyOperation(testy, m_dy);
    butterflyOperation(testz, m_dz);

    normalMap();
}

void Ocean::draw(float deltaTime, glm::vec3 cameraPosition, glm::mat4 proj, glm::mat4 view, glm::mat4 model, int tiling, bool wireframeMode)
{
    m_ocean_shader->use();

    m_vao->bind();

    m_ocean_shader->setMat4("u_proj", proj);
    m_ocean_shader->setMat4("u_view", view);
    m_ocean_shader->setMat4("u_model", model);
    m_ocean_shader->setInt("u_dx", 0);
    m_dx->bindActive(0);
    m_ocean_shader->setInt("u_dy", 1);
    m_dy->bindActive(1);
    m_ocean_shader->setInt("u_dz", 2);
    m_dz->bindActive(2);
    m_ocean_shader->setInt("u_normalMap", 3);
    m_normalMap->bindActive(3);
    m_ocean_shader->setInt("skybox", 4);
    m_ocean_shader->setFloat("u_displacement", m_spectrumParams.displacementScale);
    m_ocean_shader->setVec2("u_choppiness", m_spectrumParams.choppinessScale);

    // lightining/shadows
    m_ocean_shader->setVec3("u_viewPosition", cameraPosition);
    m_ocean_shader->setBool("u_wireframeMode", wireframeMode);

    m_ocean_shader->setFloat("u_roughness", m_material.roughness);
    m_ocean_shader->setVec3("u_sunDirection", m_material.sunDirection);
    m_ocean_shader->setVec3("u_sunIrradiance", m_material.sunIrradiance);
    m_ocean_shader->setVec3("u_scatterColor", m_material.scatterColor);
    m_ocean_shader->setVec3("u_bubbleColor", m_material.bubbleColor);
    m_ocean_shader->setFloat("u_bubbleDensity", m_material.bubbleDensity);
    m_ocean_shader->setFloat("u_wavePeakScatterStrength", m_material.wavePeakScatterStrength);
    m_ocean_shader->setFloat("u_scatterStrength", m_material.scatterStrength);
    m_ocean_shader->setFloat("u_scatterShadowStrength", m_material.scatterShadowStrength);
    m_ocean_shader->setFloat("u_waveHeight", m_material.heightWave);
    m_ocean_shader->setFloat("u_envLightStrength", m_material.envLightStrength);

    for(int j = 0; j < tiling; ++j)
    {
        for(int i = 0; i < tiling; ++i)
        {
            model = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
            model = glm::translate(model, glm::vec3(1024 * i, 0, 1024 * -j));
            m_ocean_shader->setMat4("u_model", model);
            glDrawElements(GL_TRIANGLES, m_ebo->count(), GL_UNSIGNED_INT, nullptr);
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

void Ocean::setwindDirection(const glm::vec2& windDirection)
{
    m_spectrumParams.windDirection = windDirection;
    m_recalculateSpectrum = true;
}

const glm::vec2& Ocean::windDirection() const
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

void Ocean::setSuppresorFactor(float suppresorFactor)
{
    m_spectrumParams.suppresorFactor = suppresorFactor;
    m_recalculateSpectrum = true;
}

void Ocean::setSunDirection(const glm::vec3 &sunDirection)
{
    m_material.sunDirection = sunDirection;
}

void Ocean::setSunIrradiance(const glm::vec3 &sunIrradiance)
{
    m_material.sunIrradiance = sunIrradiance;
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

void Ocean::setChoppinessScale(const glm::vec2& choppinessScale)
{
    m_spectrumParams.choppinessScale = choppinessScale;
}

void Ocean::setDisplacementScale(float displacementScale)
{
    m_spectrumParams.displacementScale = displacementScale;
}

GLuint Ocean::texture(TextureVis textureVis) const
{
    switch (textureVis)
    {
        case HIILDE0K: return m_tilde_h0k->id();
        case HIILDE0MINUSK: return m_tilde_h0minusk->id();
        case HIILDEKTDX: return m_tilde_hkt_dx->id();
        case HIILDEKTDY: return m_tilde_hkt_dy->id();
        case HIILDEKTDZ: return m_tilde_hkt_dz->id();
        case DX: return m_dx->id();
        case DY: return m_dy->id();
        case DZ: return m_dz->id();
        case NORMALMAP: return m_normalMap->id();
    }
    return 0;
}
