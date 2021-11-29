#pragma once
namespace engine
{
	class mesh;
	/// \brief Class creating a potion object with a mesh of a specified size
	class potion
	{
	public:
		/// \brief Constructor
		potion();
		/// \brief Destructor
		~potion();
		/// \brief Getter methods
		ref<engine::mesh> mesh() const { return m_mesh; }
		static ref<potion> create();
	private:
		/// \brief Fields
		// vertices of the potion specified by a vector of positions.
		ref<engine::mesh> m_mesh;
	};
}
