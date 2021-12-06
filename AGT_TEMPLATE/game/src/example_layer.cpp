#include "example_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"
#include <glm/glm/gtx/string_cast.hpp>

example_layer::example_layer() 
    :m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f), 
    m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height())


{
    // Hide the mouse and lock it inside the window
    //engine::input::anchor_mouse(true);
    engine::application::window().hide_mouse_cursor();

	//Set the start screen position of the camera

	// Initialise audio and play background music
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/bounce.wav", engine::sound_type::spatialised, "bounce"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/DST-impuretechnology.mp3", engine::sound_type::track, "music");  // Royalty free music from http://www.nosoapradio.us/
	m_audio_manager->play("music");
	m_audio_manager->pause("music");

	m_gameStart = false;
	m_display_options = false;
	freeCam = true;
	m_active_spell = false;
	m_active_spell_timer = 0.0f;

	m_3d_camera.position(glm::vec3(0, -20.f, 13.0));

	// Initialise the shaders, materials and lights
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

	// set color texture unit
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gColorMap", 0);
	m_directionalLight.submit(mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gMatSpecularIntensity", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);

	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
		(float)engine::application::window().height()));
	m_material = engine::material::create(1.0f, glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	m_mannequin_material = engine::material::create(1.0f, glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	// Skybox texture from https://opengameart.org/content/spacemoon-skybox
	m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/skybox/jpgindigo_bk.jpg", true), //ft
		  engine::texture_2d::create("assets/textures/skybox/jpgindigo_rt.jpg", true), //rt
		  engine::texture_2d::create("assets/textures/skybox/jpgindigo_ft.jpg", true), //bk 
		  engine::texture_2d::create("assets/textures/skybox/jpgindigo_lf.jpg", true), //lf
		  engine::texture_2d::create("assets/textures/skybox/jpgindigo_up.jpg", true), //up
		  engine::texture_2d::create("assets/textures/skybox/jpgindigo_dn.jpg", true) //dn
		});

	engine::ref<engine::skinned_mesh> m_skinned_mesh = engine::skinned_mesh::create("assets/models/animated/mannequin/free3Dmodel.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/walking.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/idle.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/jump.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/standard_run.dae");
	m_skinned_mesh->switch_root_movement(false);

	engine::game_object_properties mannequin_props;
	mannequin_props.animated_mesh = m_skinned_mesh;
	mannequin_props.scale = glm::vec3(1.f / glm::max(m_skinned_mesh->size().x,glm::max(m_skinned_mesh->size().y, m_skinned_mesh->size().z)));
	mannequin_props.position = glm::vec3(0.f, 0.9f, 10.f);
	mannequin_props.rotation_amount = engine::PI;
	mannequin_props.rotation_axis = glm::vec3(0.f, 1.f, 0.f);
	mannequin_props.type = 0;
	mannequin_props.bounding_shape = glm::vec3(m_skinned_mesh->size().x / 4.f,m_skinned_mesh->size().y / 2.f, m_skinned_mesh->size().x / 4.f);
	mannequin_props.friction = 0.0f;

	m_mannequin = engine::game_object::create(mannequin_props);
	m_mannequin->set_offset(m_skinned_mesh->offset());

	m_player.initialise(m_mannequin);
	m_player.set_box(mannequin_props.bounding_shape.x * 2.f * mannequin_props.scale.x,
		mannequin_props.bounding_shape.y * 2.f * mannequin_props.scale.x,
		mannequin_props.bounding_shape.z * 2.f * mannequin_props.scale.x,
		mannequin_props.position);

	/* Load the terrain textureand create a terrain mesh.Create a terrain object.Set its properties
	Texture downloaded from https://unsplash.com/photos/TmETG2KY59M */
	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/spaceTerrain.jpg", false) };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.5f, 100.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.position = glm::vec3(0.f);
	terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	terrain_props.restitution = 0.92f;
	m_terrain = engine::game_object::create(terrain_props);

	// Texture downloaded from https://pixabay.com/illustrations/not-dead-dead-mummy-warrior-sword-4849850/
	std::vector<engine::ref<engine::texture_2d>> intro_texture = { engine::texture_2d::create("assets/textures/introScreen.jpg", false) };
	engine::ref<engine::terrain> intro_shape = engine::terrain::create(10.f, 0.5f, 6.f);
	engine::game_object_properties intro_props;
	intro_props.position = glm::vec3(0.f,-20.f,0.f);
	intro_props.meshes = { intro_shape->mesh() };
	intro_props.textures = intro_texture;
	intro_props.is_static = true;
	intro_props.type = 0;
	intro_props.bounding_shape = glm::vec3(10.f, 0.5f, 6.f);
	intro_props.rotation_amount = glm::half_pi<float>();
	intro_props.rotation_axis = glm::vec3(1,0,0);
	intro_props.restitution = 0.92f;
	m_intro = engine::game_object::create(intro_props);

	std::vector<engine::ref<engine::texture_2d>> options_texture = { engine::texture_2d::create("assets/textures/optionScreen.jpg", false) };
	engine::ref<engine::terrain> options_shape = engine::terrain::create(10.f, 0.5f, 6.f);
	engine::game_object_properties options_props;
	options_props.position = glm::vec3(21.f, -20.f, 0.f);
	options_props.meshes = { options_shape->mesh() };
	options_props.textures = options_texture;
	options_props.is_static = true;
	options_props.type = 0;
	options_props.bounding_shape = glm::vec3(10.f, 0.5f, 6.f);
	options_props.rotation_amount = glm::half_pi<float>();
	options_props.rotation_axis = glm::vec3(1, 0, 0);
	options_props.restitution = 0.92f;
	m_options = engine::game_object::create(options_props);

	// Load the cow model. Create a cow object. Set its properties
	engine::ref<engine::model>cow_model =
		engine::model::create("assets/models/static/cow4.3ds");
	engine::game_object_properties cow_props;
	cow_props.meshes = cow_model->meshes();
	cow_props.textures = cow_model->textures();
	float cow_scale = 1.f / glm::max(cow_model->size().x, glm::max(cow_model->size().y,
		cow_model->size().z));
	cow_props.position = { 2.f,0.9f, 2.f };
	cow_props.scale = glm::vec3(cow_scale);
	cow_props.bounding_shape = cow_model->size() / 2.f;
	cow_props.type = 0;
	m_cow = engine::game_object::create(cow_props);
	m_cow->set_offset(cow_model->offset());
	m_cow_box.set_box(cow_props.bounding_shape.x * 2.f * cow_scale,
		cow_props.bounding_shape.y * 2.f * cow_scale, cow_props.bounding_shape.z * 2.f *
		cow_scale, cow_props.position - glm::vec3(0.f, m_cow->offset().y, 0.f) * m_cow->scale());
	m_cow = engine::game_object::create(cow_props);

	m_enemy.initialise(m_cow, cow_props.position, glm::vec3(1.f, 0.f, 0.f));


	// Mesh downloaded from https://www.turbosquid.com/3d-models/3d-model-wooden-barrels-1488970
	engine::ref <engine::model> barrel_model = engine::model::create("assets/models/static/barrel_obj.obj");
	engine::game_object_properties barrel_props;
	barrel_props.meshes = barrel_model->meshes();
	barrel_props.textures = barrel_model->textures();
	float barrel_scale = 1.f / glm::max(barrel_model->size().x, glm::max(barrel_model->size().y, barrel_model->size().z));
	barrel_props.position = { 0.f,0.9f, 0.f };
	barrel_props.scale = glm::vec3(barrel_scale);
	barrel_props.bounding_shape = barrel_model->size() / 2.f;
	barrel_props.type = 0;
	m_barrel = engine::game_object::create(barrel_props);
	m_barrel->set_offset(barrel_model->offset());
	m_barrel_box.set_box(barrel_props.bounding_shape.x * 2.f * barrel_scale,
		barrel_props.bounding_shape.y * 2.f * barrel_scale, barrel_props.bounding_shape.z * 2.f *
		barrel_scale, barrel_props.position - glm::vec3(0.f, m_barrel->offset().y, 0.f) * m_barrel->scale());

	// Mesh downloaded from https://www.turbosquid.com/3d-models/free-sword-3d-model/797355
	engine::ref <engine::model> sword_model = engine::model::create("assets/models/static/Swords-separated.obj");
	engine::game_object_properties sword_props;
	sword_props.meshes = sword_model->meshes();
	sword_props.textures = sword_model->textures();
	float sword_scale = 1.f / glm::max(sword_model->size().x, glm::max(sword_model->size().y, sword_model->size().z));
	sword_props.position = { 0.f,0.5f, 0.f };
	sword_props.scale = glm::vec3(sword_scale);
	sword_props.bounding_shape = sword_model->size() / 2.f * sword_scale;
	m_sword = engine::game_object::create(sword_props);


	// Load the tree model. Create a tree object. Set its properties
	//engine::ref <engine::model> tree_model = engine::model::create("assets/models/static/elm.3ds");
	//engine::game_object_properties tree_props;
	//tree_props.meshes = tree_model->meshes();
	//tree_props.textures = tree_model->textures();
	//float tree_scale = 3.f / glm::max(tree_model->size().x, glm::max(tree_model->size().y, tree_model->size().z));
	//tree_props.position = { 4.f, 0.5f, -5.f };
	//tree_props.bounding_shape = tree_model->size() / 2.f * tree_scale;
	//tree_props.scale = glm::vec3(tree_scale);
	//m_tree = engine::game_object::create(tree_props);

	//engine::ref<engine::sphere> spell_shape = engine::sphere::create(10, 20, 0.1f);
	engine::ref <engine::model> spell_shape = engine::model::create("assets/models/static/newSoccerBall.3ds");
	engine::game_object_properties spell_props;
	spell_props.position = { 0.f, 5.f, -5.f };
	spell_props.meshes = spell_shape->meshes();
	spell_props.textures = spell_shape->textures();
	spell_props.type = 1;
	spell_props.bounding_shape = spell_shape->size();
	spell_props.restitution = 0.85f;
	spell_props.mass = 0.45f;
	spell_props.rolling_friction = 0.1f;
	m_spell = engine::game_object::create(spell_props);

	float radius = 0.01f;
	engine::ref<engine::sphere>sphere_shape = engine::sphere::create(10, 20, radius);
	engine::game_object_properties sphere_props;
	sphere_props.position = { 0.f, 3.f, 1.f };
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.type = 1;
	sphere_props.bounding_shape = glm::vec3(radius);
	sphere_props.restitution = 0.92f;
	sphere_props.mass = 0.01f;
	m_ball = engine::game_object::create(sphere_props);

	std::vector<glm::vec3> tetrahedron_vertices;
	tetrahedron_vertices.push_back(glm::vec3(0.f, 10.f, 0.f)); //0
	tetrahedron_vertices.push_back(glm::vec3(0.f, 0.f, 10.f)); //1

	tetrahedron_vertices.push_back(glm::vec3(-10.f, 0.f, -10.f)); //2
	tetrahedron_vertices.push_back(glm::vec3(10.f, 0.f, -10.f)); //3
	engine::ref<engine::tetrahedron> tetrahedron_shape =
		engine::tetrahedron::create(tetrahedron_vertices);
	engine::game_object_properties tetrahedron_props;
	tetrahedron_props.position = { 0.f, 0.5f, -20.f };
	tetrahedron_props.meshes = { tetrahedron_shape->mesh() };
	m_tetrahedron = engine::game_object::create(tetrahedron_props);

	engine::ref<engine::potion> potion_shape =
		engine::potion::create();
	engine::game_object_properties potion_props;
	potion_props.position = { 0.f, 0.f, 0.f };
	potion_props.meshes = { potion_shape->mesh() };
	potion_props.scale = glm::vec3(.1f, .1f, .1f);
	m_potion = engine::game_object::create(potion_props);

	engine::ref<engine::table> table_shape =
		engine::table::create();
	engine::game_object_properties table_props;
	table_props.position = { 0.f, 0.f, 0.f };
	table_props.meshes = { table_shape->mesh() };
	table_props.scale = glm::vec3(.1f, .1f, .1f);
	m_table = engine::game_object::create(table_props);

	engine::ref<engine::torch> torch_shape =
		engine::torch::create();
	engine::game_object_properties torch_props;
	torch_props.position = { 0.f, 0.f, 0.f };
	torch_props.meshes = { torch_shape->mesh() };
	m_torch = engine::game_object::create(torch_props);


	m_game_objects.push_back(m_terrain);

	m_game_objects.push_back(m_intro);

	m_game_objects.push_back(m_options);

	m_game_objects.push_back(m_barrel);
	m_game_objects.push_back(m_sword);

	//m_game_objects.push_back(m_ball);
	m_game_objects.push_back(m_spell);
	m_game_objects.push_back(m_mannequin);
	//m_game_objects.push_back(m_cow);
	//m_game_objects.push_back(m_tree);
	//m_game_objects.push_back(m_pickup);
	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create();

	m_skinned_mesh->switch_animation(1);

	m_arcane_blast.initialise(m_spell);


}

example_layer::~example_layer() {}

void example_layer::on_update(const engine::timestep& time_step) 
{
	if (m_gameStart)
	{
		if (freeCam)
		{
			m_3d_camera.on_update(time_step);
		}
		else
		{
			m_player.update_fp_camera(m_3d_camera);
		}
	}

	m_player.on_update(time_step);

	m_enemy.on_update(time_step, m_player.object()->position());

		
	/*m_player_box.on_update(m_player.object()->position() - glm::vec3(0.f,
		m_player.object()->offset().y, 0.f) * m_player.object()->scale(),
		m_player.object()->rotation_amount(), m_player.object()->rotation_axis());*/

	m_cow_box.on_update(m_cow->position() - glm::vec3(0.f, m_cow->offset().y, 0.f)
		* m_cow->scale(), m_cow->rotation_amount(), m_cow->rotation_axis());

	m_barrel_box.on_update(m_barrel->position() - glm::vec3(0.f, m_barrel->offset().y, 0.f)
		* m_barrel->scale(), m_barrel->rotation_amount(), m_barrel->rotation_axis());

	m_active_spell_timer += time_step;

	if (m_active_spell_timer > 2.f)
	{
		m_active_spell = false;
		m_active_spell_timer = 0.f;
	}

	m_arcane_blast.on_update(time_step);

	m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

	m_audio_manager->update_with_camera(m_3d_camera);

	check_bounce();

	if (m_spell->is_colliding() && m_spell->collision_objects().size() > 1)
	{
		std::cout << "collision is taking place";
	}
} 

void example_layer::on_render() 
{
    engine::render_command::clear_color({0.2f, 0.3f, 0.3f, 1.0f}); 
    engine::render_command::clear();

	// Set up  shader. (renders textures and materials)
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(m_3d_camera, mesh_shader);

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	// Position the skybox centred on the player and render it
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(mesh_shader, m_skybox, skybox_tranform);

	engine::renderer::submit(mesh_shader, m_terrain);

	m_barrel_box.on_render(2.5f, 0.f, 0.f, mesh_shader);


	m_player.getBox().on_render(2.5f, 0.f, 0.f, mesh_shader);

	m_cow_box.on_render(2.5f, 0.f, 0.f, mesh_shader);

	engine::renderer::submit(mesh_shader, m_barrel);

	engine::renderer::submit(mesh_shader, m_intro);

	engine::renderer::submit(mesh_shader, m_options);



	/*glm::mat4 tree_transform(1.0f);
	tree_transform = glm::translate(tree_transform, glm::vec3(4.f, 0.5, -5.0f));
	tree_transform = glm::rotate(tree_transform, m_tree->rotation_amount(), m_tree->rotation_axis());
	tree_transform = glm::scale(tree_transform, m_tree->scale());
	engine::renderer::submit(mesh_shader, tree_transform, m_tree);*/
	
	glm::mat4 cow_transform(1.0f);
	cow_transform = glm::translate(cow_transform, m_cow->position());
	cow_transform = glm::rotate(cow_transform, m_cow->rotation_amount(), m_cow->rotation_axis());
	cow_transform = glm::scale(cow_transform, m_cow->scale());
	engine::renderer::submit(mesh_shader, cow_transform, m_cow);

	m_arcane_blast.on_render(mesh_shader);

	m_material->submit(mesh_shader);
	//engine::renderer::submit(mesh_shader, m_spell);
	//engine::renderer::submit(mesh_shader, m_tetrahedron);
	//for (auto j = 0; j < 6; j++)
	//{
	//	for (auto i = 0; i < 6; i++)
	//	{
	//		glm::mat4 potion_transform(1.0f);
	//		potion_transform = glm::translate(potion_transform, glm::vec3(i + 5.f, 1.f, j -2.f));
	//		potion_transform = glm::scale(potion_transform, glm::vec3(.1f));
	//		potion_transform = glm::rotate(potion_transform, i * 1.f * j, glm::vec3(1.f, 1.f, 1.f));
	//		engine::renderer::submit(mesh_shader, potion_transform, m_potion);
	//	}
	//}
	/*glm::mat4 potion_transform(1.0f);
	potion_transform = glm::translate(potion_transform, glm::vec3(5.f, .5f, -2.f));
	potion_transform = glm::scale(potion_transform, glm::vec3(.1f));
	potion_transform = glm::rotate(potion_transform, -.9f, glm::vec3(0.f, 1.f, 0.f));
	engine::renderer::submit(mesh_shader, potion_transform, m_potion);*/

	/*for (auto j = 0; j < 6; j++)
	{
		for (auto i = 0; i < 6; i++)
		{
			glm::mat4 table_transform(1.0f);
			table_transform = glm::translate(table_transform, glm::vec3(i - 5.f * 3, 2.f, 3 * j + 2.f));
			table_transform = glm::rotate(table_transform, i * 1.f * j, glm::vec3(1.f, 1.f, 1.f));
			table_transform = glm::scale(table_transform, glm::vec3(.01f, .01, .02f));
			engine::renderer::submit(mesh_shader, table_transform, m_table);
		}
	}*/
	/*std::vector<glm::vec3> barrelStack{
		glm::vec3(0.f, 0.9f, -5.f),
		glm::vec3(-1.f, 0.9f, -5.f),
		glm::vec3(-0.5f, 1.5f, -5.f),
	};

	for (auto i = 0; i < barrelStack.size(); i++)
	{
		glm::mat4 barrel_transform(1.0f);
		barrel_transform = glm::translate(barrel_transform, barrelStack[i]);
		barrel_transform = glm::rotate(barrel_transform, glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f));
		barrel_transform = glm::scale(barrel_transform, glm::vec3(.01f));
		engine::renderer::submit(mesh_shader, barrel_transform, m_barrel);
	}

	glm::mat4 z_barrel_transform(1.0f);
	z_barrel_transform = glm::translate(z_barrel_transform, glm::vec3(1.5f, 0.9f, -4.5f));
	z_barrel_transform = glm::rotate(z_barrel_transform, glm::half_pi<float>(), glm::vec3(0.f, 0.f, 1.f));
	z_barrel_transform = glm::scale(z_barrel_transform, glm::vec3(.01f));
	engine::renderer::submit(mesh_shader, z_barrel_transform, m_barrel);

	glm::mat4 y_barrel_transform(1.0f);
	y_barrel_transform = glm::translate(y_barrel_transform, glm::vec3(2.f, 0.5f, -4.5f));
	y_barrel_transform = glm::scale(y_barrel_transform, glm::vec3(.01f));
	engine::renderer::submit(mesh_shader, y_barrel_transform, m_barrel);

	glm::mat4 yy_barrel_transform(1.0f);
	yy_barrel_transform = glm::translate(yy_barrel_transform, glm::vec3(2.f, 1.5f, -4.5f));
	yy_barrel_transform = glm::scale(yy_barrel_transform, glm::vec3(.01f));
	engine::renderer::submit(mesh_shader, yy_barrel_transform, m_barrel);*/

	glm::mat4 sword_transform(1.0f);
	sword_transform = glm::translate(sword_transform, m_player.equipment_position());
	sword_transform = glm::rotate(sword_transform, glm::atan(m_3d_camera.front_vector().x, m_3d_camera.front_vector().z), glm::vec3(0.f, 1.f, 0.f));
	sword_transform = glm::scale(sword_transform, glm::vec3(.1f));
	engine::renderer::submit(mesh_shader,sword_transform, m_sword);


	/*glm::mat4 table_transform(1.0f);z
	table_transform = glm::translate(table_transform, glm::vec3(-5.f, .5f, -2.f));
	table_transform = glm::rotate(table_transform, -.9f, glm::vec3(0.f, 1.f, 0.f));
	table_transform = glm::scale(table_transform, glm::vec3(.01f, .01, .02f));
	engine::renderer::submit(mesh_shader, table_transform, m_table);*/

	/*glm::mat4 torch_transform(1.0f);
	torch_transform = glm::translate(torch_transform, glm::vec3(0.f, 1.5f, 0.f));
	torch_transform = glm::rotate(torch_transform, glm::pi<float>(), glm::vec3(0.f, 1.f, 0.f));
	torch_transform = glm::rotate(torch_transform, glm::half_pi<float>(), glm::vec3(-1.f, 0.f, 0.f));
	torch_transform = glm::scale(torch_transform, glm::vec3(.1f));
	engine::renderer::submit(mesh_shader, torch_transform, m_torch);*/

	/*glm::mat4 torch_two_transform(1.0f);
	torch_two_transform = glm::translate(torch_two_transform, glm::vec3(0.f, 1.5f, 0.f));
	torch_two_transform = glm::scale(torch_two_transform, glm::vec3(.1f));
	engine::renderer::submit(mesh_shader, torch_two_transform, m_torch);*/

	/*glm::mat4 torch_two_transform(1.0f);
	torch_two_transform = glm::translate(torch_two_transform, glm::vec3(2.f, 1.5f, 0.f));
	torch_two_transform = glm::scale(torch_two_transform, glm::vec3(.1f));
	engine::renderer::submit(mesh_shader, torch_two_transform, m_torch);*/

	/*for (auto j = 0; j < 6; j++)
	{
		for (auto i = 0; i < 6; i++)
		{
			glm::mat4 torch_two_transform(1.0f);
			torch_two_transform = glm::translate(torch_two_transform, glm::vec3(i + 10.f, 2.f, 3 * j + 10.f));
			torch_two_transform = glm::rotate(torch_two_transform, i * 1.f * j, glm::vec3(1.f, 1.f, 1.f));
			torch_two_transform = glm::scale(torch_two_transform, glm::vec3(.1f));
			engine::renderer::submit(mesh_shader, torch_two_transform, m_torch);
		}
	}*/

	m_mannequin_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_player.object());

	m_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_ball);



    engine::renderer::end_scene();

	// Render text
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	m_text_manager->render_text(text_shader, "Camera Position: " + glm::to_string(m_3d_camera.position()), 10.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
	m_text_manager->render_text(text_shader, "Camera front-vector: " + glm::to_string(m_3d_camera.front_vector()), 10.f, (float)engine::application::window().height() - 50.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
} 

void example_layer::on_event(engine::event& event) 
{ 
    if(event.event_type() == engine::event_type_e::key_pressed) 
    { 
        auto& e = dynamic_cast<engine::key_pressed_event&>(event); 
        if(e.key_code() == engine::key_codes::KEY_TAB) 
        { 
            engine::render_command::toggle_wireframe();
        }
		if (e.key_code() == engine::key_codes::KEY_SPACE && !m_gameStart && !m_display_options)
		{
			m_gameStart = true;
			m_3d_camera.position(glm::vec3(0,1.f,0));
		}
		if (e.key_code() == engine::key_codes::KEY_RIGHT && !m_gameStart && !m_display_options)
		{
			m_display_options = true;
			m_3d_camera.position(glm::vec3(21.f, -20.2f, 13.0));
		}
		if (e.key_code() == engine::key_codes::KEY_LEFT && !m_gameStart && m_display_options)
		{
			m_display_options = false;
			m_3d_camera.position(glm::vec3(0, -20.f, 13.0));
		}
		if (e.key_code() == engine::key_codes::KEY_J)
		{
			freeCam = !freeCam;
		}
		if (e.key_code() == engine::key_codes::KEY_N && !m_active_spell)
		{
			m_active_spell = true;
			m_arcane_blast.throw_spell(m_3d_camera, 100.0f);
			//m_audio_manager->play("throw_spell");
		}
		if (e.key_code() == engine::key_codes::KEY_BACKSPACE)
		{
			m_game_objects.erase(m_game_objects.begin());
		}
    } 
}

void example_layer::check_bounce()
{
	if (m_prev_sphere_y_vel < 0.1f && m_spell->velocity().y > 0.1f)
		//m_audio_manager->play("bounce");
		//m_audio_manager->play_spatialised_sound("bounce", m_3d_camera.position(), glm::vec3(m_spell->position().x, 0.f, m_spell->position().z));
	m_prev_sphere_y_vel = m_game_objects.at(1)->velocity().y;
}
