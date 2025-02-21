#ifndef SKYBOX_H
#define SKYBOX_H

#include <memory>
#include <vector>

#include "VArray.h"
#include "VBuffer.h"
#include "Shader.h"
#include "Texture.h"

namespace gfx
{
	// Textures from https://opengameart.org/content/sky-box-sunny-day
	class Skybox
	{
	public:
		Skybox();

		const TexturePtr &texture() const;

		void draw(const glm::mat4 &view, const glm::mat4 &projection) const;

	private:
		void createBox();

		void createTexture();

		VArrayPtr m_vao;
		VBufferPtr m_vbo;
		ShaderPtr m_shader;
		TexturePtr m_texture;

		std::vector<GLfloat> m_vertices;
	};
}

#endif //SKYBOX_H
