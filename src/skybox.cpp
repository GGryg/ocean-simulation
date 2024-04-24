#include "skybox.h"
#include "resourceLoader.h"

#include <string>

Skybox::Skybox()
{
    createBox();
    m_shader = ResourceLoader::get().loadShader("shaders/skybox_vs.glsl", "shaders/skybox_fs.glsl");
    m_vao = std::make_unique<VArray>();
    m_vao->bind();
    m_vbo = std::make_unique<VBuffer>(m_vertices.data(), m_vertices.size() * sizeof(GLfloat), GL_STATIC_DRAW);
    VBufferLayout elements;
    elements.emplace_back(3, GL_FLOAT, GL_FALSE, 0);
    m_vao->addBuffer(m_vbo.get(), 3 * sizeof(GLfloat), elements);
    createTexture();
    m_texture->bind();
    m_texture->clampToEdgeCube();
    m_texture->bilinearFilter();
    m_shader->use();
    m_shader->setInt("skybox", 6);
}

void Skybox::draw(const glm::mat4& view, const glm::mat4& projection) const
{
    glDepthFunc(GL_LEQUAL);
    m_shader->use();
    m_shader->setMat4("view", view);
    m_shader->setMat4("projection", projection);
    m_vao->bind();
    m_texture->bindActive(6);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    m_vao->unbind();
    glDepthFunc(GL_LESS);
}

const std::unique_ptr<Texture>& Skybox::texture() const
{
    return m_texture;
}

void Skybox::createBox()
{
    m_vertices = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
}

void Skybox::createTexture()
{
    std::vector<std::string> faces = {
        "resources/textures/Daylight_Box_Right.bmp",
        "resources/textures/Daylight_Box_Left.bmp",
        "resources/textures/Daylight_Box_Top.bmp",
        "resources/textures/Daylight_Box_Bottom.bmp",
        "resources/textures/Daylight_Box_Front.bmp",
        "resources/textures/Daylight_Box_Back.bmp"
    };

    m_texture = ResourceLoader::get().loadCubemap(faces);
}