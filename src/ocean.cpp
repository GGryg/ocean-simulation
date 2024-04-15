#include "ocean.h"
#include "resourceLoader.h"
#include "constants.h"

#include <cstddef>
#include <iostream>
#include <GLFW/glfw3.h>

Ocean::Ocean(int N_t, float amplitude_t, float windSpeed_t, glm::vec2 windDirection_t, float length_t)
    : m_N{N_t}
    , m_N1{N_t + 1}
    , m_amplitude{amplitude_t}
    , m_windSpeed{windSpeed_t}
    , m_windDirection{glm::normalize(windDirection_t)}
    , m_length{length_t}
    , m_log_2_N{static_cast<int>(std::log(m_N) / std::log(2))}
{
    //m_shader = ResourceManager::get().loadShader("ocean", "shaders/ocean.vs", "shaders/ocean.fs");
    //m_shader.use();
    generateMesh();

    //m_vao.bind();
    //m_vbo.addData(m_vertices.data(), m_vertices.size() * sizeof(OceanVertex), GL_STATIC_DRAW);
    //VBufferLayout layout;
    //layout.addElement<GLfloat>(3);
    //layout.addElement<GLfloat>(2);
    //m_vao.bind();
    //m_vbo.bind();
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OceanVertex), (void*)0);
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(OceanVertex), (void*)sizeof(glm::vec3));
    //m_ebo = EBuffer{m_indices.data(), m_indices.size(), GL_STATIC_DRAW};

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

    m_noise0 = std::unique_ptr<Texture>(ResourceLoader::get().loadTexture("resources/noise/noise0.png", false));
    m_noise0->bind();
    m_noise0->clampToEdge();
    m_noise0->neareastFilter();
    m_noise0->unbind();
    m_noise1 = std::unique_ptr<Texture>(ResourceLoader::get().loadTexture("resources/noise/noise1.png", false));
    m_noise1->bind();
    m_noise1->clampToEdge();
    m_noise1->neareastFilter();
    m_noise1->unbind();
    m_noise2 = std::unique_ptr<Texture>(ResourceLoader::get().loadTexture("resources/noise/noise2.png", false));
    m_noise2->bind();
    m_noise2->clampToEdge();
    m_noise2->neareastFilter();
    m_noise2->unbind();
    m_noise3 = std::unique_ptr<Texture>(ResourceLoader::get().loadTexture("resources/noise/noise3.png", false));
    m_noise3->bind();
    m_noise3->clampToEdge();
    m_noise3->neareastFilter();
    m_noise3->unbind();


    m_tilde_h0k_program = ResourceLoader::get().loadShader("shaders/h_0_k_cs.glsl");

    m_tilde_h0k = std::make_unique<Texture>(GL_TEXTURE_2D, m_N, m_N, GL_RGBA32F, GL_RGBA);
    m_tilde_h0k->bind();
    //m_tilde_h0k->texImage2D();
    m_tilde_h0k->allocateStorage(1);
    m_tilde_h0k->clampToEdge();
    m_tilde_h0k->neareastFilter();
    //m_tilde_h0k->allocateStorage(1);
    m_tilde_h0k->unbind();
    m_tilde_h0minusk = std::make_unique<Texture>(GL_TEXTURE_2D, m_N, m_N, GL_RGBA32F, GL_RGBA);
    m_tilde_h0minusk->bind();
    m_tilde_h0minusk->allocateStorage(1);
    m_tilde_h0minusk->clampToEdge();
    m_tilde_h0minusk->neareastFilter();
    m_tilde_h0minusk->unbind();

    m_tilde_hkt_program = ResourceLoader::get().loadShader("shaders/h_k_t_cs.glsl");

    m_tilde_hkt_dx = std::make_unique<Texture>(GL_TEXTURE_2D, m_N, m_N, GL_RGBA32F, GL_RGBA);
    m_tilde_hkt_dx->bind();
    m_tilde_hkt_dx->allocateStorage(1);
    m_tilde_hkt_dx->repeat();
    m_tilde_hkt_dx->bilinearFilter();
    m_tilde_hkt_dx->unbind();
    m_tilde_hkt_dy = std::make_unique<Texture>(GL_TEXTURE_2D, m_N, m_N, GL_RGBA32F, GL_RGBA);
    m_tilde_hkt_dy->bind();
    m_tilde_hkt_dy->allocateStorage(1);
    m_tilde_hkt_dy->repeat();
    m_tilde_hkt_dy->bilinearFilter();
    m_tilde_hkt_dy->unbind();
    m_tilde_hkt_dz = std::make_unique<Texture>(GL_TEXTURE_2D, m_N, m_N, GL_RGBA32F, GL_RGBA);
    m_tilde_hkt_dz->bind();
    m_tilde_hkt_dz->allocateStorage(1);
    m_tilde_hkt_dz->repeat();
    m_tilde_hkt_dz->bilinearFilter();
    m_tilde_hkt_dz->unbind();

    m_twiddleFactors_program = ResourceLoader::get().loadShader("shaders/twiddle_factors_cs.glsl");

    m_twiddleFactors = std::make_unique<Texture>(GL_TEXTURE_2D, m_log_2_N, m_N, GL_RGBA32F, GL_RGBA);
    m_twiddleFactors->bind();
    m_twiddleFactors->allocateStorage(1);
    m_twiddleFactors->clampToEdge();
    m_twiddleFactors->neareastFilter();
    m_twiddleFactors->unbind();

    m_butterflyOperation_program = ResourceLoader::get().loadShader("shaders/butterfly_cs.glsl");
    m_inversion_program = ResourceLoader::get().loadShader("shaders/inversion_cs.glsl");

    m_pingPong = std::make_unique<Texture>(GL_TEXTURE_2D, m_N, m_N, GL_RGBA32F, GL_RGBA);
    m_pingPong->bind();
    m_pingPong->allocateStorage(1);
    m_pingPong->unbind();

    m_dx = std::make_unique<Texture>(GL_TEXTURE_2D, m_N, m_N, GL_RGBA32F, GL_RGBA);
    m_dx->bind();
    m_dx->allocateStorage(1);
    m_dx->unbind();
    m_dy = std::make_unique<Texture>(GL_TEXTURE_2D, m_N, m_N, GL_RGBA32F, GL_RGBA);
    m_dy->bind();
    m_dy->allocateStorage(1);
    m_dy->unbind();
    m_dz = std::make_unique<Texture>(GL_TEXTURE_2D, m_N, m_N, GL_RGBA32F, GL_RGBA);
    m_dz->bind();
    m_dz->allocateStorage(1);
    m_dz->unbind();

    m_normalMap_program = ResourceLoader::get().loadShader("shaders/normal_map_cs.glsl");

    m_normalMap = std::make_unique<Texture>(GL_TEXTURE_2D, m_N, m_N, GL_RGBA32F, GL_RGBA);
    m_normalMap->bind();
    m_normalMap->allocateStorage(1);
    m_normalMap->repeat();
    m_normalMap->bilinearFilter();
    m_normalMap->unbind();

    tilde_h0k();
    calculateTwiddleFactor();
}

Ocean::~Ocean()
{

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

float Ocean::phillipsSepctrum(int n, int m)
{
    // k = (kx, kz), kx = 2*PI*n/Lx, kz = 2PI*m/Lz
    // -N/2 <= n < N/2, same with m
    glm::vec2 k(std::numbers::pi_v<float> * 2 * n / m_length, std::numbers::pi_v<float> * 2 * m / m_length);
    float kLength = glm::length(k);
    float kLength_2 = kLength * kLength;
    float kLength_4 = kLength_2 * kLength_2;
    // Ph(k) = A * ((exp(-1/(kL)^2))/k^4) * |kconj dot wconj|^2
    return 0.0f;
}

void Ocean::tilde_h0k()
{
    m_tilde_h0k_program.use();
    m_noise0->bindActive(2);
    m_tilde_h0k_program.setInt("noise0", 2);
    m_noise1->bindActive(3);
    m_tilde_h0k_program.setInt("noise1", 3);
    m_noise2->bindActive(4);
    m_tilde_h0k_program.setInt("noise2", 4);
    m_noise3->bindActive(5);
    m_tilde_h0k_program.setInt("noise3", 5);

    m_tilde_h0k_program.setFloat("u_amplitude", m_amplitude);
    m_tilde_h0k_program.setFloat("u_windSpeed", m_windSpeed);
    m_tilde_h0k_program.setVec2("u_WindDirection", m_windDirection);
    m_tilde_h0k_program.setInt("u_N", m_N);
    m_tilde_h0k_program.setInt("u_L", m_length);

    m_tilde_h0k->bindImage(0, 0, 0, GL_WRITE_ONLY);
    m_tilde_h0minusk->bindImage(1, 0, 0, GL_WRITE_ONLY);

    glDispatchCompute(32, 32, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glFinish();
}

void Ocean::tilde_hkt(float deltaTime)
{
    m_tilde_hkt_program.use();

    m_tilde_h0k->bindImage(0, 0, 0, GL_READ_ONLY);
    m_tilde_h0minusk->bindImage(1, 0, 0, GL_READ_ONLY);
    m_tilde_hkt_dx->bindImage(2, 0, 0, GL_WRITE_ONLY);
    m_tilde_hkt_dy->bindImage(3, 0, 0, GL_WRITE_ONLY);
    m_tilde_hkt_dz->bindImage(4, 0, 0, GL_WRITE_ONLY);

    m_tilde_hkt_program.setInt("u_N", m_N);
    m_tilde_hkt_program.setInt("u_L", m_length);
    m_tilde_hkt_program.setFloat("u_t", static_cast<float>(glfwGetTime()));

    glDispatchCompute(8, 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

GLuint Ocean::reverseBits(GLuint n)
{
    GLuint result = 0;
    int numBits = static_cast<int>(std::log(m_N) / std::log(2));
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

    for(int i = 0; i < m_N; ++i)
    {
        indices.push_back(reverseBits(i));
    }
    m_ssbo = std::make_unique<SSBuffer>(indices.data(), indices.size() * sizeof(GLuint), GL_STATIC_DRAW);
}

void Ocean::calculateTwiddleFactor()
{
    m_twiddleFactors_program.use();

    m_twiddleFactors->bindImage(0, 0, 0, GL_WRITE_ONLY);
    m_ssbo->bindBase(1);

    m_twiddleFactors_program.setInt("u_N", m_N);

    glDispatchCompute(m_log_2_N, 8, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glFinish();

}

void Ocean::butterflyOperation(std::unique_ptr<Texture>& input, std::unique_ptr<Texture>& output)
{
    m_butterflyOperation_program.use();
    m_twiddleFactors->bindImage(0, 0, 0, GL_READ_ONLY);
    input->bindImage(1, 0, 0, GL_READ_WRITE);
    m_pingPong->bindImage(2, 0, 0, GL_READ_WRITE);

    int pingpong = 0;
    for(int i = 0; i < m_log_2_N; i++)
    {
        m_butterflyOperation_program.setInt("u_pingpong", pingpong);
        m_butterflyOperation_program.setInt("u_isVertical", 0);
        m_butterflyOperation_program.setInt("u_stage", i);

        glDispatchCompute(8, 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        pingpong ^= 1;
    }

    for(int i = 0; i < m_log_2_N; i++)
    {
        m_butterflyOperation_program.setInt("u_pingpong", pingpong);
        m_butterflyOperation_program.setInt("u_isVertical", 1);
        m_butterflyOperation_program.setInt("u_stage", i);

        glDispatchCompute(8, 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        pingpong ^= 1;
    }

    m_inversion_program.use();

    m_inversion_program.setInt("u_pingpong", pingpong);
    m_inversion_program.setInt("u_N", m_N);

    output->bindImage(0, 0, 0, GL_WRITE_ONLY);
    input->bindImage(1, 0, 0, GL_READ_ONLY);
    m_pingPong->bindImage(2, 0, 0, GL_READ_ONLY);

    glDispatchCompute(8, 8, 1);
    glFinish();
}

void Ocean::normalMap()
{
    m_normalMap_program.use();
    m_normalMap->bindImage(0, 0, 0, GL_WRITE_ONLY);

    m_normalMap_program.setInt("u_dy", 1);
    m_dy->bindActive(1);
    
    m_normalMap_program.setInt("u_N", m_N);

    glDispatchCompute(8, 8, 1);
    glFinish();
}

void Ocean::waving(float deltaTime)
{
    tilde_hkt(deltaTime);
    butterflyOperation(m_tilde_hkt_dy, m_dy);
    butterflyOperation(m_tilde_hkt_dx, m_dx);
    butterflyOperation(m_tilde_hkt_dz, m_dz);

    normalMap();
}

void Ocean::draw(float deltaTime, glm::vec3 lightPosition, glm::vec3 cameraPosition, glm::mat4 proj, glm::mat4 view, glm::mat4 model)
{
    m_vao->bind();
}
