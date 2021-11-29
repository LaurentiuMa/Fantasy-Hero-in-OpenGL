#pragma once
namespace engine
{
	class mesh;
	/// \brief Class creating a potion object with a mesh of a specified size
	class table
	{
	public:
		/// \brief Constructor
		table();
		/// \brief Destructor
		~table();
		/// \brief Getter methods
		ref<engine::mesh> mesh() const { return m_mesh; }
		static ref<table> create();
	private:
		/// \brief Fields
		// vertices of the potion specified by a vector of positions.
		ref<engine::mesh> m_mesh;
	};
}
