#pragma once
#include <engine.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "player.h"
#include "arcane_blast.h"
#include "engine/entities/bounding_box_bullet.h"
#include "enemy.h"
#include "mimic.h"
#include "lemur.h"
#include "cross_fade.h"
#include "lightning_bolt.h"
#include "billboard.h"



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
	engine::ref<engine::game_object>	m_mimic{};
	engine::ref<engine::game_object>	m_lemur{};
	engine::ref<engine::game_object>	m_tree{};
	engine::ref<engine::game_object>	m_spell{};
	engine::ref<engine::game_object>	m_mannequin{};
	engine::ref<engine::game_object>	m_monk{};
	engine::ref<engine::game_object>	m_tetrahedron{};
	engine::ref<engine::game_object>	m_potion{};
	engine::ref<engine::game_object>	m_table{};
	engine::ref<engine::game_object>	m_torch{};
	engine::ref<engine::game_object>	m_torchLight{};
	engine::ref<engine::game_object>	m_intro{};
	engine::ref<engine::game_object>	m_options{};
	engine::ref<engine::game_object>	m_barrel{};
	engine::ref<engine::game_object>	m_sword{};
	engine::ref<engine::game_object>	m_ball{};
	engine::ref<engine::game_object>	m_grenadePickup{};
	engine::ref<engine::game_object>	m_lightningPickup{};

	engine::ref<engine::game_object>	m_westEntranceWall{};
	engine::ref<engine::game_object>	m_eastEntranceWall{};
	engine::ref<engine::game_object>	m_topEntranceWall{};
	engine::ref<engine::game_object>	m_topMainWall{};
	engine::ref<engine::game_object>	m_southMainWall{};
	engine::ref<engine::game_object>	m_southInnerWall{};
	engine::ref<engine::game_object>	m_southAngledWall{};
	engine::ref<engine::game_object>	m_southEastWall{};
	engine::ref<engine::game_object>	m_mainEastWall{};
	engine::ref<engine::game_object>	m_mainNorthWall{};
	engine::ref<engine::game_object>	m_mainWestWall{};
	engine::ref<engine::game_object>	m_splitterWall{};

	player								m_player{};

	enemy								m_enemy{};
	mimic								m_mimicNPC{};
	lemur								m_lemurNPC{};


	arcane_blast m_arcane_blast;

	engine::bounding_box				m_barrel_box;
	engine::bounding_box				m_cow_box;
	engine::bounding_box				m_lemur_box;
	engine::bounding_box				m_mimic_box;
	engine::bounding_box				m_potion_box;


	engine::ref<engine::material>		m_material{};
	engine::ref<engine::material>		m_mannequin_material{};
	engine::ref<engine::material>		m_lightsource_material{};


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

	float								wallLength, wallDepth, wallWidth;

	glm::vec3							enemy_pos;
	glm::vec3							grenadeSpawn;

	engine::ref<cross_fade>							m_cross_fade{};
	std::vector<engine::ref<lightning_bolt>>		m_lightning_bolts{};
	engine::ref<billboard>							m_billboard{};


	engine::PointLight					m_pointLight_three;
	uint32_t							num_point_lights = 1;

	engine::SpotLight					m_intro_spotLight;
	uint32_t							num_spot_lights = 1;

	bool								healingAvailable;
	bool								spawnPotion;
	bool								spawnGrenade;
	bool								lightningAvailable;
	bool								blastAvailable;
	bool								drawBoundingBoxes;
	bool								unawareMimicKilled;
	bool								grenadePickedup;
	bool								mimicAlive;
	bool								cowAlive;
	bool								lemurAlive;
	bool								lightningPickup;

	float								boltRotation;
};
