#pragma once
#include <engine.h>
class lemur
{
	enum class state
	{
		patrolling,
		scared
	};
public:
	lemur();
	~lemur();
	// set the parameters for the lemur
	void initialise(engine::ref<engine::game_object> object, glm::vec3 position,
		glm::vec3 forward);
	// update the lemur
	void on_update(const engine::timestep& time_step, const glm::vec3& player_position);
	// methods controlling the lemur’s behaviour in a certain state
	void patrol(const engine::timestep& time_step);

	void run_away(const engine::timestep& time_step, const glm::vec3&
		player_position);
	glm::vec3 changeForward();
	// game object bound to the lemur
	engine::ref<engine::game_object> object() const { return m_object; }
private:
	// lemur's speed
	float m_speed{ 0.1f };
	// timer controlling the direction switch and the reset value for this timer
	float m_default_time{ 4.f };
	float m_switch_direction_timer = m_default_time;
	// threshold distances
	float m_detection_radius{ 4.f };
	// game object bound to the lemur
	engine::ref< engine::game_object> m_object;
	//current state of the lemur's state machine
	state m_state = state::patrolling;

};
