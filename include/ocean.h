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
    glm::vec2 texCoord; // maybe unused
};

/*
struct OceanVertexHelper
{
    GLfloat h_x, h_y, h_z;
    GLfloat h_conj_x, hconj_y, hconj_z;
    GLfloat ox, oy, oz;
};
*/

class Ocean
{
public:
    Ocean(int N_t, float amplitude_t, float windSpeed_t, glm::vec2 windDirection_t, float length_t);
    ~Ocean();

    float phillipsSepctrum(int n, int m);

    void tilde_h0k();
    void tilde_htk(float t, int n, int m);

    void waving(float deltaTime); // update
    void draw(float deltaTime, glm::vec3 lightPosition, glm::vec3 cameraPosition, glm::mat4 proj, glm::mat4 view, glm::mat4 model);

public:
    void generateMesh();
    void reverseIndices();
    GLuint reverseBits(GLuint n);

    int m_N{};
    int m_N1{};
    float m_amplitude{};
    float m_windSpeed;
    glm::vec2 m_windDirection{};
    float m_length;

    std::unique_ptr<Texture> m_noise0;
    std::unique_ptr<Texture> m_noise1;
    std::unique_ptr<Texture> m_noise2;
    std::unique_ptr<Texture> m_noise3;

    Shader m_tilde_h0k_program;

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