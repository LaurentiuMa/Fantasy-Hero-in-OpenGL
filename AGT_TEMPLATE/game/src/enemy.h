#pragma once
#include <engine.h>

enum class enemy_state
{
	patrolling,
	on_guard,
	chasing,
	resting,
	attacking
};

class enemy
{
public:
	enemy();
	~enemy();
	// set the parameters for the enemy
	void initialise(engine::ref<engine::game_object> object, glm::vec3 position,
		glm::vec3 forward);
	// update the enemy
	void on_update(const engine::timestep& time_step, const glm::vec3& player_position);
	// methods controlling the enemy’s behaviour in a certain state
	void patrol(const engine::timestep& time_step);

	void face_player(const engine::timestep& time_step, const glm::vec3&
		player_position);
	void chase_player(const engine::timestep& time_step, const glm::vec3&
		player_position);
	bool rest(const engine::timestep& time_step);

	enemy_state getState() { return m_state; };

	void takeDamage(int dmg) { health -= dmg; };

	int getHealth() { return health; };

	// game object bound to the enemy
	engine::ref<engine::game_object> object() const { return m_object; }
private:
	// enemy's speed
	float m_speed{ 0.1f };
	// timer controlling the direction switch and the reset value for this timer
	float m_default_time{ 4.f };
	float m_switch_direction_timer = m_default_time;
	// threshold distances
	float m_detection_radius{ 6.f };
	float m_trigger_radius{ 4.f };
	float m_attacking_radius{ 3.f };
	// game object bound to the enemy
	engine::ref< engine::game_object> m_object;
	//current state of the enemy's state machine
	enemy_state m_state = enemy_state::patrolling;

	std::vector<glm::vec3> patrolForwardVectors{
		glm::vec3(1,0,0),
		glm::vec3(-1,0,-1),
		glm::vec3(-1,0,1)
	};
	int patrolDirectionTimeCount = 0;
	std::vector<int> patrolTime{
		8,
		4,
		4
	};

	float defaultrestTime = 4;
	float restTime = defaultrestTime;
	float restBeginTime;
	glm::vec3 velocityBegin;

	int health;

};
