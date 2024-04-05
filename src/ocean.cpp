#include "ocean.h"
#include "resourceManager.h"

Ocean::Ocean(int N_t, float amplitude_t, glm::vec2 windDirection_t, float length_t)
    : m_N{N_t}
    , m_N1{N_t + 1}
    , m_amplitude{amplitude_t}
    , m_windDirection{windDirection_t}
    , m_length{length_t}
{
    h_tilde = VecCompf(m_N * m_N);
    h_tilde_x = VecCompf(m_N * m_N);
    h_tilde_z = VecCompf(m_N * m_N);
    h_tilde_dx = VecCompf(m_N * m_N);
    h_tilde_dz = VecCompf(m_N * m_N);

    m_fft{m_N};

    m_vertices = std::vector<OceanVertex>(m_N1 * m_N1);
    m_verticesHelper = std::vector<OceanVertexHelper>(m_N1 * m_N1);

    m_shader = ResourceManager::loadShader("shaders/ocean.vs", "shaders/ocean.fs");

    m_vao{};
    VBuffer{m_vertices, m_vertices.size() * sizeof(OceanVertex), GL_DYNAMIC_DRAW};
    VBufferLayout layout;
    layout.addElement<GLfloat>(3);
    layout.addElement<GLfloat>(3);
    vao.addBuffer(vbo, layout);
}

Ocean::~Ocean()
{
    
}