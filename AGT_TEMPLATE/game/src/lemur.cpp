#include "pch.h"
#include "lemur.h"
lemur::lemur() {}
lemur::~lemur() {}
void lemur::initialise(engine::ref<engine::game_object> object, glm::vec3 position,
	glm::vec3 forward)
{
	m_object = object;
	m_object->set_forward(forward);
	m_object->set_position(position);
}
void lemur::on_update(const engine::timestep& time_step, const glm::vec3& player_position)
{
	float distance_to_player = glm::distance(m_object->position(), player_position);
	// check which state is the lemur in, then execute the matching behaviour
	if (m_state == state::patrolling)
	{
		patrol(time_step);
		// check whether the condition has been met to switch to the on_guard state
		if (distance_to_player < m_detection_radius)
			m_state = state::scared;
			//run_away(time_step, player_position);
	}
	else if (m_state == state::scared)
	{
		run_away(time_step, player_position);
		if (distance_to_player > m_detection_radius)
		{
			m_state = state::patrolling;
		}
	}
	
}
// move forwards until the timer runs out, then switch direction to move the other way

void lemur::patrol(const engine::timestep& time_step)
{
	m_switch_direction_timer -= (float)time_step;
	if (m_switch_direction_timer < 0.f)
	{
		m_object->set_forward(changeForward());
		m_switch_direction_timer = m_default_time;
	}

	m_object->set_position(m_object->position() + m_object->forward() * m_speed *
		(float)time_step);
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}

// move forwards in the direction of the player
void lemur::run_away(const engine::timestep& time_step, const glm::vec3&
	player_position)
{
	m_object->set_forward((player_position + m_object->position()) * glm::vec3(1.f,0,1.f));
	m_object->set_position(m_object->position() + (m_object->forward() / 7.f) * m_speed *
		(float)time_step);
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}

glm::vec3 lemur::changeForward()
{
	float x = rand() % 2 - 1;
	float z = rand() % 2 - 1;

	return glm::vec3(x, 0, z);


}
