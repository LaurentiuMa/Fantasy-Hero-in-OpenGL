#pragma once
#include <engine.h>
#include "quad.h"

class quad;

class hud
{
public:
	hud(const std::string& path, float width, float height);
	~hud();

	void on_render(engine::ref<engine::shader> shader);

	static engine::ref<hud> create(const std::string& path, float width, float height);
	engine::ref<engine::text_manager>	m_text_manager{};

private:

	engine::ref<engine::texture_2d> m_texture;
	engine::ref<quad> m_quad;
};

