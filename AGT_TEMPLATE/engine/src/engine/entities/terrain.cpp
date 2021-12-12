#include "pch.h"
#include "terrain.h"

engine::terrain::terrain(float length, float depth, float width, float repeat) : cuboid(glm::vec3(length, depth, width), false, repeat), m_length(length), m_depth(depth), m_width(width), m_repeat(repeat) {}

engine::terrain::~terrain() {}

engine::ref<engine::terrain> engine::terrain::create(float length, float depth, float width, float repeat)
{
	return std::make_shared<engine::terrain>(length, depth, width, repeat);
}
