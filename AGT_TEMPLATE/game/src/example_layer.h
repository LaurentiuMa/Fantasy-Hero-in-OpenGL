#pragma once
#include <engine.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "player.h"
#include "arcane_blast.h"
#include "engine/entities/bounding_box_bullet.h"
#include "enemy.h"


class example_layer : public engine::layer
{
public:
    example_layer();
	~example_layer();

    void on_update(const engine::timestep& time_step) override;
    void on_render() override; 
    void on_event(engine::event& event) override;

	void resetPropPosition(engine::game_object_properties props);

private:
	void check_bounce();


	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::game_object>	m_terrain{};
	engine::ref<engine::game_object>	m_cow{};
	engine::ref<engine::game_object>	m_tree{};
	engine::ref<engine::game_object>	m_spell{};
	engine::ref<engine::game_object>	m_mannequin{};
	engine::ref<engine::game_object>	m_tetrahedron{};
	engine::ref<engine::game_object>	m_potion{};
	engine::ref<engine::game_object>	m_table{};
	engine::ref<engine::game_object>	m_torch{};
	engine::ref<engine::game_object>	m_intro{};
	engine::ref<engine::game_object>	m_options{};
	engine::ref<engine::game_object>	m_barrel{};
	engine::ref<engine::game_object>	m_sword{};
	engine::ref<engine::game_object>	m_ball{};

	engine::ref<engine::game_object>	m_westEntranceWall{};
	engine::ref<engine::game_object>	m_eastEntranceWall{};
	engine::ref<engine::game_object>	m_topEntranceWall{};
	engine::ref<engine::game_object>	m_topMainWall{};
	engine::ref<engine::game_object>	m_southMainWall{};

	player								m_player{};

	enemy								m_enemy{};

	arcane_blast m_arcane_blast;

	engine::bounding_box				m_barrel_box;
	engine::bounding_box				m_cow_box;

	engine::ref<engine::material>		m_material{};
	engine::ref<engine::material>		m_mannequin_material{};

	engine::DirectionalLight            m_directionalLight;

	std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	float								m_prev_sphere_y_vel = 0.f;
	engine::ref<engine::text_manager>	m_text_manager{};

    engine::orthographic_camera			m_2d_camera; 
    engine::perspective_camera			m_3d_camera;

	bool								m_gameStart;
	bool								m_display_options;
	bool								freeCam;
	bool								m_active_spell;

	float								m_active_spell_timer;

	float wallLength, wallDepth, wallWidth;
};
