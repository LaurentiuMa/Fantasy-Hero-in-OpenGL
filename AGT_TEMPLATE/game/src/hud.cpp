#include "pch.h"
#include "hud.h"
#include <glm/gtc/type_ptr.hpp>

hud::hud(const std::string& path, float width, float height)
{
	m_texture = engine::texture_2d::create(path, true);
	m_quad = quad::create(glm::vec2(width, height));
}

hud::~hud(){}

void hud::on_render(engine::ref<engine::shader> shader)
{

	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(1.3f, 0.7f, 0.1f));

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_texture->bind();
	engine::renderer::submit(shader, m_quad->mesh(), transform);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);

}

engine::ref<hud> hud::create(const std::string& path, float width, float height)
{
	return std::make_shared<hud>(path, width, height);
}
