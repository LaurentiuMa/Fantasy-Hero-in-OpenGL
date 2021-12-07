#pragma once
#include <engine.h>

class quadrilateral
{
public:
	/// \brief Constructor
	quadrilateral(glm::vec2 half_extents);

	/// \brief Destructor
	~quadrilateral();

	/// \brief Getter methods
	glm::vec2 half_extents() const { return m_half_extents; }
	engine::ref<engine::mesh> mesh() const { return m_mesh; }

	static engine::ref<quadrilateral> create(glm::vec2 half_extents);
private:
	/// \brief Fields
	// Dimensions of the quad specified by a vector of half-extents. Cuboid is centered on the origin in local coordinates.
	glm::vec2 m_half_extents;

	engine::ref<engine::mesh> m_mesh;
};


