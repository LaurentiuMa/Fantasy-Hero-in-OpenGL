#include "pch.h"
#include "player.h"
#include "engine/core/input.h"
#include "engine/key_codes.h"
#include <engine/renderer/camera.cpp>

player::player()
{
	m_timer = 0.0f;
	m_speed = 0.0f;
	cam_wobble = 0.0f;
	wobble_modifier = 0.01f;
}
player::~player() {};

void player::initialise(engine::ref<engine::game_object> object)
{
	m_object = object;
	m_object->set_forward(glm::vec3(0.f, 0.f, -1.f));
	m_object->set_position(glm::vec3(0.f, 0.9f, 10.f));	m_object->animated_mesh()->set_default_animation(1);
	m_object->set_angular_factor_lock(true);
	health = 100;
}

void player::on_update(const engine::timestep& time_step)
{
	m_object->animated_mesh()->on_update(time_step);

	//m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));

	if (engine::input::key_pressed(engine::key_codes::KEY_1)) { // left

		turn(1.0f * time_step + m_speed_boost/1000);
	}
	else if (engine::input::key_pressed(engine::key_codes::KEY_2)) { // right
		turn(-1.0f * time_step - m_speed_boost/1000);
	}
	else { turn(0.0f); }

	if (engine::input::key_pressed(engine::key_codes::KEY_O))
	{
		wobble();
		float down_vel = m_object->velocity().y;
		m_object->set_velocity(glm::vec3(m_object->forward().x * (m_speed + m_speed_boost), down_vel, m_object -> forward().z * (m_speed + m_speed_boost)));
	}
	else if (engine::input::key_pressed(engine::key_codes::KEY_P))
	{
		wobble();
		float down_vel = m_object->velocity().y;
		m_object->set_velocity(glm::vec3(-m_object->forward().x * (m_speed + m_speed_boost), down_vel, -m_object -> forward().z * (m_speed + m_speed_boost)));
	}
	else
	{
		m_object->set_velocity(m_object->forward() * 0.f);
	}


	if (engine::input::key_pressed(engine::key_codes::KEY_H)) // right

		jump();

	if (m_timer > 0.0f)
	{
		m_timer -= (float)time_step;
		if (m_timer < 0.0f)
		{
			m_object->animated_mesh()->switch_root_movement(false);
			m_object->animated_mesh()->switch_animation(m_object->animated_mesh() -> default_animation());
			m_speed = 1.0f;
		}
	}

	m_player_box.on_update(object()->position() - glm::vec3(0.f, object()->offset().y,
		0.f) * object()->scale(), object()->rotation_amount(), object()->rotation_axis());

}

void player::turn(float angle)
{
	/*m_object->set_forward(glm::rotate(m_object->forward(), angle, glm::vec3(0.f, 1.f,
		0.f)));*/
	m_object->set_angular_velocity(glm::vec3(0.f, angle * 100.f, 0.f));
}

void player::update_tp_camera(engine::perspective_camera& camera)
{
	camera.up_vector(glm::vec3(0,1,0));
	float camPosAbove = 2.f;
	float camPosBehind = 3.f;
	float camLookAheadAt = 6.f;

	glm::vec3 camViewPoint = m_object->position() + (camLookAheadAt * m_object->forward());
	glm::vec3 cameraPostion = m_object->position() - camPosBehind * m_object->forward() + glm::vec3(0,camPosAbove,0);

	camera.set_view_matrix(cameraPostion, camViewPoint);
}

void player::update_fp_camera(engine::perspective_camera& camera)
{

	float camPosAbove = 0.0f + cam_wobble;
	float camPosFront = 0.2f;
	float camLookAheadAt = 6.f;

	glm::vec3 camViewPoint = m_object->position() + (camLookAheadAt * m_object->forward());
	glm::vec3 cameraPostion = m_object->position() + camPosFront * m_object->forward() + glm::vec3(0, camPosAbove, 0);
	camera.up_vector(glm::vec3(0,1.f,0));
	camera.set_view_matrix(cameraPostion, camViewPoint);
}

void player::jump()
{
	m_object->animated_mesh()->switch_root_movement(true);
	m_object->animated_mesh()->switch_animation(3);
	m_speed = 0.5f;


	m_timer = m_object->animated_mesh()->animations().at(3)->mDuration;

}

glm::vec3 player::equipment_position()
{
	float equipmentPosAbove = 0.2f;
	float equipmentPosFront = 0.5f;

	return m_object->position() + equipmentPosFront * m_object->forward() + glm::vec3(0, equipmentPosAbove, 0);
}

// Added a wobble effect that takes place whenever the player is walking
void player::wobble()
{
	m_speed = 1.0f;

	cam_wobble += wobble_modifier;

	if (cam_wobble >= 0.6f) wobble_modifier = -0.002f;
	if (cam_wobble <= 0.4f) wobble_modifier = 0.002f;
}
