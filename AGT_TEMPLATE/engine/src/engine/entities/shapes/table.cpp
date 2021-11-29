#include "pch.h"
#include "table.h"
#include <engine.h>
#include <algorithm>

engine::table::table()
{
	//Vertices for the (-x,z) table leg
	std::vector<mesh::vertex> table
	{
		//Front
		// position			normal			tex coord       
		{ {-34,0,34},		{0,0,1},		{0 ,0} },
		{ {-30,0,34},		{0,0,1},		{1 ,0} },
		{ {-30,70,34},		{0,0,1},		{1 ,1} },
		{ {-34,70,34},		{0,0,1},		{0 ,1} },

		//Right
		{ {-30,0,34},		{1,0,0},		{0 ,0} },
		{ {-30,0,30},		{1,0,0},		{1 ,0} },
		{ {-30,70,30},		{1,0,0},		{1 ,1} },
		{ {-30,70,34},		{1,0,0},		{0 ,1} },

		//Back
		{ {-30,0,30},		{0,0,-1},		{0 ,0} },
		{ {-34,0,30},		{0,0,-1},		{1 ,0} },
		{ {-34,70,30},		{0,0,-1},		{1 ,1} },
		{ {-30,70,30},		{0,0,-1},		{0 ,1} },

		//Left
		{ {-34,0,30},		{-1,0,0},		{0 ,0} },
		{ {-34,0,34},		{-1,0,0},		{1 ,0} },
		{ {-34,70,34},		{-1,0,0},		{1 ,1} },
		{ {-34,70,30},		{-1,0,0},		{0 ,1} },

		//Top
		{ {-34,70,34},		{0,1,0},		{0 ,0} },
		{ {-30,70,34},		{0,1,0},		{1 ,0} },
		{ {-30,70,30},		{0,1,0},		{1 ,1} },
		{ {-34,70,30},		{0,1,0},		{0 ,1} },

		//Bottom
		{ {-34,0,30},		{0,-1,0},		{0 ,0} },
		{ {-30,0,30},		{0,-1,0},		{1 ,0} },
		{ {-30,0,34},		{0,-1,0},		{1 ,1} },
		{ {-34,0,34},		{0,-1,0},		{0 ,1} },
	};

	std::vector<mesh::vertex> tableTop
	{
		//front
		{ {-38,70,38},		{0,0,1},		{0 ,0} },
		{ {38,70,38},		{0,0,1},		{1 ,0} },
		{ {38,76,38},		{0,0,1},		{1 ,1} },
		{ {-38,76,38},		{0,0,1},		{0 ,1} },

		//right
		{ {38,70,38},		{1,0,0},		{0 ,0} },
		{ {38,70,-38},		{1,0,0},		{1 ,0} },
		{ {38,76,-38},		{1,0,0},		{1 ,1} },
		{ {38,76,38},		{1,0,0},		{0 ,1} },

		//back
		{ {38,70,-38},		{0,0,-1},		{0 ,0} },
		{ {-38,70,-38},		{0,0,-1},		{1 ,0} },
		{ {-38,76,-38},		{0,0,-1},		{1 ,1} },
		{ {38,76,-38},		{0,0,-1},		{0 ,1} },

		//left
		{ {-38,70,-38},		{-1,0,0},		{0 ,0} },
		{ {-38,70,38},		{-1,0,0},		{1 ,0} },
		{ {-38,76,38},		{-1,0,0},		{1 ,1} },
		{ {-38,76,-38},		{-1,0,0},		{0 ,1} },

		//top
		{ {-38,76,38},		{0,1,0},		{0 ,0} },
		{ {38,76,38},		{0,1,0},		{1 ,0} },
		{ {38,76,-38},		{0,1,0},		{1 ,1} },
		{ {-38,76,-38},		{0,1,0},		{0 ,1} },

		//bottom
		{ {-38,70,-38},		{0,-1,0},		{0 ,0} },
		{ {38,70,-38},		{0,-1,0},		{1 ,0} },
		{ {38,70,38},		{0,-1,0},		{1 ,1} },
		{ {-38,70,38},		{0,-1,0},		{0 ,1} },
	};

	//FINAL INDICES MUST BE PASSED AS CONST!!!!!!!!!!!!!!!!!!!!!!!!!!
	std::vector<uint32_t> table_indices
	{
		 0,  1,  2,		 0,  2,  3,  //front
		 4,  5,  6,		 4,  6,  7,  //right
		 8,  9, 10,		 8, 10, 11,  //back
		12, 13, 14,		12, 14, 15,  //left
		16, 17, 18,		16, 18, 19,  //upper
		20, 21, 22,		20, 22, 23,  //bottom

		24, 25, 26,		24, 26, 27,  //front
		28, 29, 30,		28, 30, 31,  //right
		32, 33, 34,		32, 34, 35,  //back
		36, 37, 38,		36, 38, 39,  //left
		40, 41, 42,		40, 42, 43,  //upper
		44, 45, 46,		44, 46, 47   //bottom
	};

	std::vector<mesh::vertex> bottom_right_foot_vertices = table;
	std::vector<mesh::vertex> top_left_foot_vertices = table;
	std::vector<mesh::vertex> top_right_foot_vertices = table;
	std::vector<uint32_t> bottom_right_foot_indices = table_indices;
	std::vector<uint32_t> top_left_foot_indices = table_indices;
	std::vector<uint32_t> top_right_foot_indices = table_indices;

	for (auto i = 0; i < table.size(); ++i)
	{	
		bottom_right_foot_vertices[i].position.x = bottom_right_foot_vertices[i].position.x * -1;
		top_left_foot_vertices[i].position.z = top_left_foot_vertices[i].position.z * -1;

		top_right_foot_vertices[i].position.x = top_right_foot_vertices[i].position.x * -1;
		top_right_foot_vertices[i].position.z = top_right_foot_vertices[i].position.z * -1;
		
		/*std::cout << bottom_right_foot[i].position.x * -1 << '\n';*/
	}

	table.insert(table.end(), tableTop.begin(), tableTop.end());
	table.insert(table.end(), bottom_right_foot_vertices.begin(), bottom_right_foot_vertices.end());
	table.insert(table.end(), top_left_foot_vertices.begin(), top_left_foot_vertices.end());
	table.insert(table.end(), top_right_foot_vertices.begin(), top_right_foot_vertices.end());



	for (auto i = 0; i < bottom_right_foot_indices.size(); ++i)
	{
		bottom_right_foot_indices[i] += 24;
		top_left_foot_indices[i] += 48;
		top_right_foot_indices[i] += 72;
	}
	std::reverse(bottom_right_foot_indices.begin(), bottom_right_foot_indices.end());
	std::reverse(top_left_foot_indices.begin(), top_left_foot_indices.end());

	table_indices.insert(table_indices.end(), bottom_right_foot_indices.begin(), bottom_right_foot_indices.end());
	table_indices.insert(table_indices.end(), top_left_foot_indices.begin(), top_left_foot_indices.end());
	table_indices.insert(table_indices.end(), top_right_foot_indices.begin(), top_right_foot_indices.end());

	const std::vector<uint32_t> final_table_indices = table_indices;

	m_mesh = engine::mesh::create(table, final_table_indices);

}

engine::table::~table() {}

engine::ref<engine::table> engine::table::create()
{
	return std::make_shared<engine::table>();
}
