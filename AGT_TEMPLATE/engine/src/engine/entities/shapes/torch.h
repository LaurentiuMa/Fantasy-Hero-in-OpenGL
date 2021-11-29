#pragma once
namespace engine
{
	class mesh;
	/// \brief Class creating a potion object with a mesh of a specified size
	class torch
	{
	public:
		/// \brief Constructor
		torch();
		/// \brief Destructor
		~torch();
		/// \brief Getter methods
		ref<engine::mesh> mesh() const { return m_mesh; }
		static ref<torch> create();
	private:
		/// \brief Fields
		// vertices of the potion specified by a vector of positions.
		ref<engine::mesh> m_mesh;
	};
}
