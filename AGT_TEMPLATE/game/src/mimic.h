#pragma once
#include <engine.h>
class mimic
{
	enum class state
	{
		patrolling,
		aware,
	};
public:
	mimic();
	~mimic();
	// set the parameters for the mimic
	void initialise(engine::ref<engine::game_object> object, glm::vec3 position,
		glm::vec3 forward);
	// update the mimic
	void on_update(const engine::timestep& time_step, const glm::vec3& player_position);
	// methods controlling the mimic’s behaviour in a certain state
	void patrol(const engine::timestep& time_step);
	// game object bound to the mimic
	engine::ref<engine::game_object> object() const { return m_object; }
private:
	// mimic's speed
	float m_speed{ 0.1f };
	// timer controlling the direction switch and the reset value for this timer
	float m_default_time{ 4.f };
	float m_switch_direction_timer = m_default_time;
	// threshold distances
	float m_detection_radius{ 4.f };
	// game object bound to the mimic
	engine::ref< engine::game_object> m_object;
	//current state of the mimic's state machine
	state m_state = state::patrolling;

	bool is_aware;
};
