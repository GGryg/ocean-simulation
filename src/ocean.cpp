#include "ocean.h"
#include "resourceManager.h"
#include "constants.h"

Ocean::Ocean(int N_t, float amplitude_t, glm::vec2 windDirection_t, float length_t)
    : m_N{N_t}
    , m_N1{N_t + 1}
    , m_amplitude{amplitude_t}
    , m_windDirection{windDirection_t}
    , m_length{length_t}
    , m_fft{static_cast<unsigned int>(m_N)}
{
    h_tilde = VecCompf(m_N * m_N);
    h_tilde_x = VecCompf(m_N * m_N);
    h_tilde_z = VecCompf(m_N * m_N);
    h_tilde_dx = VecCompf(m_N * m_N);
    h_tilde_dz = VecCompf(m_N * m_N);

    m_shader = ResourceManager::get().loadShader("ocean", "shaders/ocean.vs", "shaders/ocean.fs");

    m_vbo = VBuffer{m_vertices.data(), m_vertices.size() * sizeof(OceanVertex), GL_DYNAMIC_DRAW};
    VBufferLayout layout;
    layout.addElement<GLfloat>(3);
    layout.addElement<GLfloat>(3);
    m_vao.addBuffer(m_vbo, layout);
}

Ocean::~Ocean()
{

}

void Ocean::generateMesh()
{
    m_vertices.reserve(constants::gridSize * constants::gridSize);
    m_indices.reserve(constants::gridSize * constants::gridSize * 3);
    const int half = constants::gridSize / 2;
    for(int x = -half; x <= half; ++x)
    {
        for(int y = -half; y <= half; ++y)
        {
            float texU = ((x + half) / static_cast<float>(constants::gridSize)) * constants::gridUVSize;
            float texV = ((y + half) / static_cast<float>(constants::gridSize)) * constants::gridUVSize;

            m_vertices.emplace_back(glm::vec3(static_cast<float>(x), 0.0f, static_cast<float>(y)), glm::vec2(texU, texV));
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