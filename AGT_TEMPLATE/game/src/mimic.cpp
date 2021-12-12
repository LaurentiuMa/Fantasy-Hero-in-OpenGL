#include "pch.h"
#include "mimic.h"
mimic::mimic() {}
mimic::~mimic() {}
void mimic::initialise(engine::ref<engine::game_object> object, glm::vec3 position,
	glm::vec3 forward)
{
	m_object = object;
	m_object->set_forward(forward);
	m_object->set_position(position);
	is_aware = false;
}
void mimic::on_update(const engine::timestep& time_step, const glm::vec3& player_position)
{
	float distance_to_player = glm::distance(m_object->position(), player_position);
	// check which state is the mimic in, then execute the matching behaviour
	if (m_state == state::patrolling)
	{
		is_aware = false;
		patrol(time_step);
		// check whether the condition has been met to switch to the aware state
		if (distance_to_player < m_detection_radius)
			m_state = state::aware;
	}
	else
	{
		is_aware = true;
		m_object->set_rotation_amount(m_object->rotation_amount() + time_step * 1.5f);
		if (distance_to_player > m_detection_radius)
		{
			m_state = state::patrolling;
		}
	}

}

// move up and down a straight line with the forward being based on a timer

void mimic::patrol(const engine::timestep& time_step)
{
	m_switch_direction_timer -= (float)time_step;
	if (m_switch_direction_timer < 0.f)
	{
		m_object->set_forward(m_object->forward() * -1.f);
		m_switch_direction_timer = m_default_time;	}

	m_object->set_position(m_object->position() + m_object->forward() * m_speed *
		(float)time_step);
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}


