#include "pch.h"
#include "enemy.h"
enemy::enemy() {}
enemy::~enemy() {}
void enemy::initialise(engine::ref<engine::game_object> object, glm::vec3 position,
	glm::vec3 forward)
{
	m_object = object;
	m_object->set_forward(forward);
	m_object->set_position(position);
	health = 100;
}
void enemy::on_update(const engine::timestep& time_step, const glm::vec3& player_position)
{
	float distance_to_player = glm::distance(m_object->position(), player_position);
	// check which state is the enemy in, then execute the matching behaviour
	if (m_state == enemy_state::patrolling)
	{
		patrol(time_step);
		// check whether the condition has been met to switch to the on_guard state
		if (distance_to_player < m_detection_radius)
			m_state = enemy_state::on_guard;
		velocityBegin = m_object->velocity();
	}
	else if (m_state == enemy_state::on_guard)
	{
		face_player(time_step, player_position);
		// check whether the condition has been met to switch back to patrolling state
		if (distance_to_player > m_detection_radius)
			m_state = enemy_state::patrolling;
		// check whether the condition has been met to switch to the chasing state
		else if (distance_to_player < m_trigger_radius)
			m_state = enemy_state::chasing;
	}
	else if(m_state == enemy_state::chasing)
	{
		m_speed = .3f;
		chase_player(time_step, player_position);
		// check whether the condition has been met to switch back to patrolling state
		if (distance_to_player > m_detection_radius)
		{
			m_speed = .1f;
			m_state = enemy_state::resting;
			restBeginTime = (float)time_step;
		}
		else if (distance_to_player < m_attacking_radius)
		{
			m_state = enemy_state::attacking;
		}
	}
	else if (m_state == enemy_state::attacking)
	{
		if (distance_to_player > m_attacking_radius)
		{
			m_state = enemy_state::chasing;
		}
	}
	else
	{
		if (rest(time_step))
		{
			m_state = enemy_state::patrolling;
			restTime = defaultrestTime;
		}
	}
}
// move forwards until the timer runs out, then switch direction to move the other way

void enemy::patrol(const engine::timestep& time_step)
{
	m_switch_direction_timer -= (float)time_step;
	if (m_switch_direction_timer < 0.f)
	{
		int forwardVectorIndex = patrolDirectionTimeCount % 3;
		/*m_object->set_forward(m_object->forward() * -1.f);*/
		m_object->set_forward(patrolForwardVectors[forwardVectorIndex]);
		m_switch_direction_timer = patrolTime[patrolDirectionTimeCount % 3];
		++patrolDirectionTimeCount;
	}

	m_object->set_position(m_object->position() + m_object->forward() * m_speed *
		(float)time_step);
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}
// stop and look at the player
void enemy::face_player(const engine::timestep& time_step, const glm::vec3&
	player_position)
{
	m_object->set_forward(player_position - m_object->position());
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}
// move forwards in the direction of the player
void enemy::chase_player(const engine::timestep& time_step, const glm::vec3&
	player_position)
{
	m_object->set_forward(player_position - m_object->position());
	m_object->set_position(m_object->position() + m_object->forward() * m_speed *
		(float)time_step);
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}

//after resting, the enemy continues seeking the player in the last direction it was seen and continues
//the routine.
bool enemy::rest(const engine::timestep& time_step)
{
	m_object->set_velocity(glm::vec3(0));
	restTime = restTime - (float)time_step;
	if (restTime < .0f)
	{
		m_object->set_velocity(velocityBegin);
		return true;
	}
	return false;
	
}
