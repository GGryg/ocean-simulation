#include "ocean.h"
#include "resourceLoader.h"
#include "constants.h"

#include <cstddef>
#include <iostream>

Ocean::Ocean(int N_t, float amplitude_t, float windSpeed_t, glm::vec2 windDirection_t, float length_t)
    : m_N{N_t}
    , m_N1{N_t + 1}
    , m_amplitude{amplitude_t}
    , m_windSpeed{windSpeed_t}
    , m_windDirection{windDirection_t}
    , m_length{length_t}
    , m_fft{static_cast<unsigned int>(m_N)}
{
    h_tilde = VecCompf(m_N * m_N);
    h_tilde_x = VecCompf(m_N * m_N);
    h_tilde_z = VecCompf(m_N * m_N);
    h_tilde_dx = VecCompf(m_N * m_N);
    h_tilde_dz = VecCompf(m_N * m_N);

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

std::complex<float> Ocean::hTilde0(int n, int m)
{
    return std::complex<float>();
}

void Ocean::draw(float deltaTime, glm::vec3 lightPosition, glm::vec3 cameraPosition, glm::mat4 proj, glm::mat4 view, glm::mat4 model)
{
    m_vao->bind();
}
