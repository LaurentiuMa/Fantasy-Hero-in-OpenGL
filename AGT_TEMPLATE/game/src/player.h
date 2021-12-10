#pragma once
#include <engine.h>
#include "glm/gtx/rotate_vector.hpp"
#include "engine/entities/bounding_box_bullet.h"

class player
{

public:
	player();
	~player();

	void initialise(engine::ref<engine::game_object> object);
	void on_update(const engine::timestep& time_step);

	engine::ref<engine::game_object> object() const { return m_object; }

	void update_tp_camera(engine::perspective_camera& camera);
	void update_fp_camera(engine::perspective_camera& camera);

	void turn(float angle);
	void jump();

	glm::vec3 equipment_position();

	void wobble();
	void increaseSpeed() {m_speed_boost += 0.5f;};
	glm::vec3 position() { return m_object->position(); };

	void set_box(float width, float height, float depth, glm::vec3 position) {
		m_player_box.set_box(width, height, depth, position);
	}
	engine::bounding_box& getBox() { return m_player_box; };

	int getHealth() { return health; };

	void setHealth(float hp) { health = hp; };

	void receivedHit(int dmg) { health -= dmg; };

	void heal() { health += 25; };

	glm::vec3 getForward() { return m_object->forward(); };

private:

	float m_speed{ 0.f };
	float m_speed_boost{ 1.f };
	float m_timer;
	float cam_wobble;
	float wobble_modifier;
	float health;

	engine::ref< engine::game_object> m_object;
	engine::bounding_box m_player_box;
};
