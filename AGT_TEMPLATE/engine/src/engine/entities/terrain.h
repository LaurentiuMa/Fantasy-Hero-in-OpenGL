#pragma once
#include "shapes/cuboid.h"

namespace engine
{
	class cuboid;

	/// \brief Class creating a terrain object containing a cuboid shape of a specified size
	class terrain : public cuboid
	{
	public:
		/// \brief Constructor
		terrain(float length, float depth, float width,float repeat);
		/// \brief Destructor
		~terrain();

		/// \brief Getter methods
		float width() const { return m_width; }
		float length() const { return m_length; }
		float depth() const { return m_depth; }

		static ref<terrain> create(float length, float depth, float width, float repeat);
	private:
		/// \brief Fields
		// terrain's width
		float m_width;
		// terrain's depth
		float m_depth;
		// terrain's length
		float m_length;

		// Changed the cuboid class definition to include a repeat
		// float which dictates if the cuboid should be repeating the
		// assigned texture. Changed all of the calls to include and
		// additional 1.f as and additional parameter.

		// Functionality is displayed when creating the ground in
		// example_layer.cpp.
		float m_repeat;
	};
}
