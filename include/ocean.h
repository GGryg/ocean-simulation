#ifndef OCEAN_H
#define OCEAN_H

#include <complex>

#include <glm/glm.hpp>
#include <GLAD/gl.h>

#include "fft.h"
#include "buffers.h"

struct OceanVertex
{
    GLfloat x, y, z; // vertex positions
    GLfloat vx, vy, vz; // normal positions
    GLfloat h_x, h_y, h_z; // texture positions
};

struct OceanVertexHelper
{
    GLfloat h_conj_x, hconj_y, hconj_z;
    GLfloat ox, oy, oz;
}

class Ocean
{
public:
    Ocean(int N_t, float amplitude_t, glm::vec2 windDirection_t, float length_t);
    ~Ocean();

    std::complex<float> hTilde0(int n, int m);
    std::complex<float> hTilde(float t, int n, int m);

    void waving(float t); // update
    void draw(float t, glm::vec3 lightPosition, glm::vec3 cameraPosition, glm::mat4 proj, glm::mat4 view, glm::mat4 model);

private:
    int m_N{};
    int m_N1{};
    float m_amplitude{};
    glm::vec2 m_windDirection{};
    float m_length;

    VecCompf h_tilde;
    VecCompf h_tilde_x;
    VecCompf h_tilde_z;
    VecCompf h_tilde_dx;
    VecCompf h_tilde_dz;

    FFT m_fft;

    std::vector<OceanVertex> m_vertices;
    std::vector<OceanVertexHelper> m_verticesHelper;
    VArray m_vao;
    VBuffer m_vbo;
    // possibly there will be a need for EBO

    Shader m_shader;
};

#endif