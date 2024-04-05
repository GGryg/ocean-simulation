#ifndef OCEAN_H
#define OCEAN_H

#include <complex>

#include <glm/glm.hpp>
#include <GLAD/gl.h>

#include "fft.h"
#include "shader.h"
#include "buffers.h"

struct OceanVertex
{
    glm::vec3 vertexPosition;
    glm::vec2 texCoord; // maybe unused
};

struct OceanVertexHelper
{
    GLfloat h_x, h_y, h_z;
    GLfloat h_conj_x, hconj_y, hconj_z;
    GLfloat ox, oy, oz;
};

class Ocean
{
public:
    Ocean(int N_t, float amplitude_t, glm::vec2 windDirection_t, float length_t);
    ~Ocean();

    float phillipsSepctrum(int n, int m);

    std::complex<float> hTilde0(int n, int m);
    std::complex<float> hTilde(float t, int n, int m);

    void waving(float t); // update
    void draw(float t, glm::vec3 lightPosition, glm::vec3 cameraPosition, glm::mat4 proj, glm::mat4 view, glm::mat4 model);

private:
    void generateMesh();

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
    std::vector<GLuint> m_indices;
    std::vector<OceanVertexHelper> m_verticesHelper;
    VArray m_vao;
    VBuffer m_vbo;
    // Create EBO

    Shader m_shader;
};

#endif