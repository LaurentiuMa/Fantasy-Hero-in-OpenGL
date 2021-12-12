#pragma once
#include <engine.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "engine/entities/bounding_box_bullet.h"
#include "player.h"
#include "arcane_blast.h"
#include "enemy.h"
#include "mimic.h"
#include "lemur.h"
#include "cross_fade.h"
#include "lightning_bolt.h"
#include "billboard.h"
#include "hud.h"


class example_layer : public engine::layer
{
public:
    example_layer();
	~example_layer();

    void on_update(const engine::timestep& time_step) override;
    void on_render() override; 
    void on_event(engine::event& event) override;

	void resetPropPosition(engine::game_object_properties props);

	void checkHP();

	void popHeart();

	void enemyAttack();

private:
	void check_bounce();

	std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	// FX
	std::vector<engine::ref<lightning_bolt>>		m_lightning_bolts{};
	engine::ref<billboard>							m_billboard{};

	// Terrain-related objects
	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::game_object>	m_terrain{};
	engine::ref<engine::game_object>	m_intro{};
	engine::ref<engine::game_object>	m_options{};
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

	// Static models
	engine::ref<engine::game_object>	m_cow{};
	engine::ref<engine::game_object>	m_mimic{};
	engine::ref<engine::game_object>	m_lemur{};
	engine::ref<engine::game_object>	m_spell{};
	engine::ref<engine::game_object>	m_barrel{};
	engine::ref<engine::game_object>	m_sword{};

	// Animated models
	engine::ref<engine::game_object>	m_mannequin{};

	// Primitive objects
	engine::ref<engine::game_object>	m_potion{};
	engine::ref<engine::game_object>	m_table{};
	engine::ref<engine::game_object>	m_torch{};
	engine::ref<engine::game_object>	m_ball{};
	engine::ref<engine::game_object>	m_torchLight{};

	// Pickup items
	engine::ref<engine::game_object>	m_grenadePickup{};
	engine::ref<engine::game_object>	m_lightningPickup{};


	// PCs & NPCs
	player								m_player{};
	enemy								m_enemy{};
	mimic								m_mimicNPC{};
	lemur								m_lemurNPC{};

	// Spells
	arcane_blast						m_arcane_blast;

	// Bounding boxes
	engine::bounding_box				m_barrel_box;
	engine::bounding_box				m_cow_box;
	engine::bounding_box				m_lemur_box;
	engine::bounding_box				m_mimic_box;
	engine::bounding_box				m_potion_box;

	// Materials
	engine::ref<engine::material>		m_material{};
	engine::ref<engine::material>		m_mannequin_material{};
	engine::ref<engine::material>		m_lightsource_material{};

	// Lights
	engine::DirectionalLight            m_directionalLight;

	engine::PointLight					m_pointLight_three;
	uint32_t							num_point_lights = 1;

	engine::SpotLight					m_intro_spotLight;
	uint32_t							num_spot_lights = 1;

	// Managers
	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	engine::ref<engine::text_manager>	m_text_manager{};

	// Camera
    engine::orthographic_camera			m_2d_camera; 
    engine::perspective_camera			m_3d_camera;

	// Cross-fade effect
	engine::ref<cross_fade>				m_cross_fade_healing{};
	engine::ref<cross_fade>				m_cross_fade_damage{};

	// HUD heart images
	engine::ref<hud>					m_hud_hundred{};
	engine::ref<hud>					m_hud_eighty{};
	engine::ref<hud>					m_hud_sixty{};
	engine::ref<hud>					m_hud_forty{};
	engine::ref<hud>					m_hud_twenty{};
	engine::ref<hud>					m_hud_zero{};
	std::vector<engine::ref<hud>>		m_heart_vector;

	// Bools
	bool								healingAvailable;
	bool								spawnPotion;
	bool								spawnGrenade;
	bool								lightningAvailable;
	bool								blastAvailable;
	bool								drawBoundingBoxes;
	bool								unawareMimicKilled;
	bool								grenadePickedup;
	bool								mimicAlive;
	bool								cowDefeated;
	bool								lemurCaught;
	bool								lightningPickup;
	bool								m_gameOver;
	bool								m_gameStart;
	bool								m_display_options;
	bool								m_display_gameOver;
	bool								freeCam;

	bool								eightyPop, sixtyPop, fortyPop, twentyPop, zeroPop;

	// Floats
	float								boltRotation;
	float								lightningRadius{4.f};
	float								enemy_attackCD = 2.f;
	float								enemy_attackTime = 0;
	float								lightningCD = 3.f;
	float								lightningTime = 0;
	float								tStep;
	float								m_gameOverTime;
	float								m_gameOverCD = .5;
	float								m_prev_sphere_y_vel = 0.f;
	float								wallLength, wallDepth, wallWidth;

	// Integers
	int									heart_pops;

	// Glm vectors
	glm::vec3							enemy_pos;
	glm::vec3							grenadeSpawn;

};
