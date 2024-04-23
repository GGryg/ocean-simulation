#ifndef SKYBOX_H
#define SKYBOX_H

#include <memory>
#include <vector>

#include <glad/gl.h>

#include "buffers.h"
#include "shader.h"
#include "texture.h"

// Texture from https://opengameart.org/content/sky-box-sunny-day
class Skybox
{
public:
    Skybox();

    void draw(const glm::mat4& view, const glm::mat4& projection) const;
private:
    void createBox();
    void createTexture();

    std::unique_ptr<VArray> m_vao;
    std::unique_ptr<VBuffer> m_vbo;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Texture> m_texture;

    std::vector<GLfloat> m_vertices;

};

#endif