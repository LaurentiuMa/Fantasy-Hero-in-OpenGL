#include "example_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"
#include <glm/glm/gtx/string_cast.hpp>

//DISCLAIMER: The variable m_cow is used for the Mob, not entirely appropriate  however it was too late to
//			  change this at the time.

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
	m_audio_manager->load_sound("assets/audio/background.wav",	engine::sound_type::track, "music");		// Royalty free music from http://www.nosoapradio.us/
	m_audio_manager->load_sound("assets/audio/birds.wav",		engine::sound_type::track, "birds");		// Royalty free sound from https://freesound.org/people/CosmicEmbers/sounds/161649/
	m_audio_manager->load_sound("assets/audio/ominous.wav",		engine::sound_type::track, "ominous");		// Royalty free sound from https://freesound.org/people/zimbot/sounds/322054/
	m_audio_manager->load_sound("assets/audio/rain.wav",		engine::sound_type::track, "rain");			// Royalty free sound from https://freesound.org/people/psuess/sounds/393483/
	m_audio_manager->load_sound("assets/audio/electricity.wav", engine::sound_type::event, "electricity");  // Royalty free sound from https://freesound.org/people/sharesynth/sounds/341666/
	m_audio_manager->load_sound("assets/audio/throw.wav",		engine::sound_type::event, "throw");		// Royalty free sound from https://freesound.org/people/marchon11/sounds/493224/
	m_audio_manager->load_sound("assets/audio/pickup.ogg",		engine::sound_type::event, "pickup");		// Royalty free sound from https://freesound.org/people/TreasureSounds/sounds/332629/
	m_audio_manager->load_sound("assets/audio/speedup.wav",		engine::sound_type::event, "speedup");		// Royalty free sound from https://freesound.org/people/Eschwabe3/sounds/460132/
	m_audio_manager->load_sound("assets/audio/healing.wav",		engine::sound_type::event, "healing");		// Royalty free sound from https://freesound.org/people/KeshaFilm/sounds/471834/

	m_audio_manager->play("music");

	m_gameStart = false;
	m_display_options = false;
	freeCam = false;
	healingAvailable = false;
	spawnPotion = true;
	lightningAvailable = false;
	lightningPickup = true;
	blastAvailable = true;
	drawBoundingBoxes = false;
	unawareMimicKilled = false;
	grenadePickedup = false;
	mimicAlive = true;
	lemurCaught = true;
	cowDefeated = true;
	spawnGrenade = false;
	m_gameOver = false;

	heart_pops = 0;
	boltRotation = 0;
	m_gameOverTime = 0.f;

	m_3d_camera.position(glm::vec3(0, -20.f, 13.0));

	// Initialise the shaders, materials and lights
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	// Create the lights
	m_directionalLight.Color = glm::vec3(0.1f, 0.5f, 0.1f);
	m_directionalLight.AmbientIntensity = 0.15f;
	m_directionalLight.DiffuseIntensity = 0.3f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.5f));

	m_pointLight_three.Color = glm::vec3(1.f, 1.0f, 1.f);
	m_pointLight_three.AmbientIntensity = 0.15f;
	m_pointLight_three.DiffuseIntensity = 0.2f;
	m_pointLight_three.Position = glm::vec3(10.82f, 3.0f, -5.97f);

	m_intro_spotLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_intro_spotLight.AmbientIntensity = 1.f;
	m_intro_spotLight.DiffuseIntensity = 0.6f;
	m_intro_spotLight.Position = glm::vec3(-3.f, -20.0f, 13.f);
	m_intro_spotLight.Direction = glm::vec3(0,0,-1);
	m_intro_spotLight.Cutoff = 0.8f;
	m_intro_spotLight.Attenuation.Constant = 1.0f;
	m_intro_spotLight.Attenuation.Linear = 0.001f;
	m_intro_spotLight.Attenuation.Exp = 0.01f;


	// Set color texture unit
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

	//material for showing a light source (mostly for debugging)
	m_lightsource_material = engine::material::create(1.0f, m_intro_spotLight.Color,
		m_intro_spotLight.Color, m_intro_spotLight.Color, 1.0f);

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
	mannequin_props.position = glm::vec3(0.f, 2.5f, 10.f);
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
	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/Cobblestone.png", false) };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.5f, 100.f, 60.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.position = glm::vec3(0.f);
	terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	terrain_props.restitution = 0.92f;
	m_terrain = engine::game_object::create(terrain_props);


	wallLength = 0.1f;
	wallDepth = 4.f;
	wallWidth = 5.f;
	// All of the walls in the scene, do not remove any of them as it will mess up the translate for the rest.
	// TODO: improve on this implementation
	std::vector<engine::ref<engine::texture_2d>> wall_textures = { engine::texture_2d::create("assets/textures/spaceTerrain.jpg", false) };
	engine::ref<engine::terrain> wall_shape = engine::terrain::create(wallLength, wallDepth, wallWidth, 1.f);
	engine::game_object_properties wall_props;
	wall_props.meshes = { wall_shape->mesh() };
	wall_props.textures = wall_textures;
	wall_props.is_static = true;
	wall_props.type = 0;
	wall_props.position = glm::vec3(0.f);
	wall_props.bounding_shape = glm::vec3(wallLength, wallDepth, wallWidth);
	wall_props.restitution = 0.92f;
	m_westEntranceWall = engine::game_object::create(wall_props);

	wall_props.position = glm::vec3(-2.f,0,0);
	m_eastEntranceWall = engine::game_object::create(wall_props);

	wall_props.rotation_amount = glm::half_pi<float>();
	wall_props.rotation_axis = glm::vec3(0,0,1);
	wall_props.position = glm::vec3(-1.f, 4.f, 0);
	m_topEntranceWall = engine::game_object::create(wall_props);

	wall_props.position = glm::vec3(5.f, 4.25f, -10.f);
	wall_props.scale = glm::vec3(2.f);
	m_topMainWall = engine::game_object::create(wall_props);
	wall_props.scale = glm::vec3(.5f);

	resetPropPosition(wall_props);

	wall_props.rotation_amount = glm::half_pi<float>();
	wall_props.rotation_axis = glm::vec3(0,1,0);
	wall_props.position = glm::vec3(2.5f, 2.f, -5.f);
	m_southMainWall = engine::game_object::create(wall_props);

	wall_props.rotation_amount = glm::half_pi<float>();
	wall_props.rotation_axis = glm::vec3(1, 0, 0);
	wall_props.position = glm::vec3(5.f, 2.f, -5.f);
	m_southInnerWall = engine::game_object::create(wall_props);

	wall_props.rotation_amount = -3 / glm::half_pi<float>();
	wall_props.rotation_axis = glm::vec3(0, 1, 0);
	wall_props.position = glm::vec3(7.4f, 2.f, -2.20f);
	m_southAngledWall = engine::game_object::create(wall_props);

	wall_props.rotation_amount = -2 / (3 * glm::half_pi<float>());
	wall_props.rotation_axis = glm::vec3(0, 1, 0);
	wall_props.position = glm::vec3(10.75f, 2.f, -3.70f);
	m_southEastWall = engine::game_object::create(wall_props);

	wall_props.scale = glm::vec3(1.f,1.f,1.4f);
	wall_props.rotation_amount = (3 * (glm::pi<float>()/2)) + glm::half_pi<float>();
	wall_props.rotation_axis = glm::vec3(0, 1, 0);
	wall_props.position = glm::vec3(11.7f, 2.f, -12.5f);
	m_mainEastWall = engine::game_object::create(wall_props);

	wall_props.scale = glm::vec3(1.f, 1.f, 1.5f);
	wall_props.rotation_amount = glm::half_pi<float>();
	wall_props.rotation_axis = glm::vec3(0, 1, 0);
	wall_props.position = glm::vec3(5.f, 2.f, -19.f);
	m_mainNorthWall = engine::game_object::create(wall_props);

	wall_props.scale = glm::vec3(1.f, 1.f, 1.4f);
	wall_props.rotation_amount = glm::pi<float>();
	wall_props.rotation_axis = glm::vec3(0, 1, 0);
	wall_props.position = glm::vec3(-2.f, 2.f, -12.f);
	m_mainWestWall = engine::game_object::create(wall_props);

	wall_props.scale = glm::vec3(1.f, 1.f, 1.5f);
	wall_props.position = glm::vec3(5.f, 2.f, -12.f);
	m_splitterWall = engine::game_object::create(wall_props);

	// Texture downloaded from https://pixabay.com/illustrations/not-dead-dead-mummy-warrior-sword-4849850/
	std::vector<engine::ref<engine::texture_2d>> intro_texture = { engine::texture_2d::create("assets/textures/introScreen.jpg", false) };
	engine::ref<engine::terrain> intro_shape = engine::terrain::create(10.f, 0.5f, 6.f, 1.f);
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

	//Options screen
	std::vector<engine::ref<engine::texture_2d>> options_texture = { engine::texture_2d::create("assets/textures/optionScreen.jpg", false) };
	engine::ref<engine::terrain> options_shape = engine::terrain::create(10.f, 0.5f, 6.f, 1.f);
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
	engine::ref<engine::model> cow_model =
		engine::model::create("assets/models/static/AnimalOBJ.obj");
	engine::game_object_properties cow_props;
	cow_props.meshes = cow_model->meshes();
	cow_props.textures = cow_model->textures();
	float cow_scale = 1.f / glm::max(cow_model->size().x, glm::max(cow_model->size().y,
		cow_model->size().z));
	cow_props.position = { 8.f,0.5f, -5.f };
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

	// Load the mimic model. Create a mimic object. Set its properties
	//downloaded from https://opengameart.org/content/mimic
	engine::ref<engine::model> mimic_model =
		engine::model::create("assets/models/static/mimic/mimic_000001.obj");
	engine::game_object_properties mimic_props;
	mimic_props.meshes = mimic_model->meshes();
	mimic_props.textures = mimic_model->textures();
	float mimic_scale = 1.f / glm::max(mimic_model->size().x, glm::max(mimic_model->size().y,
		mimic_model->size().z));
	//mimic_props.position = { 7.f,0.5f, -5.f };
	mimic_props.position = { 1.f,1.0f, -14.f };
	mimic_props.rotation_amount = glm::pi<float>();
	mimic_props.rotation_axis = glm::vec3(0,1.f,0);
	mimic_props.scale = glm::vec3(mimic_scale) ;
	mimic_props.bounding_shape = mimic_model->size() / 2.f;
	mimic_props.type = 0;
	m_mimic = engine::game_object::create(mimic_props);
	m_mimic->set_offset(mimic_model->offset());
	m_mimic_box.set_box(mimic_props.bounding_shape.x * 2.f * mimic_scale,
		mimic_props.bounding_shape.y * 2.f * mimic_scale, mimic_props.bounding_shape.z * 2.f *
		mimic_scale, mimic_props.position - glm::vec3(0.f, m_mimic->offset().y - 3.f, 0.f) * m_mimic->scale());
	m_mimic = engine::game_object::create(mimic_props);

	m_mimicNPC.initialise(m_mimic, mimic_props.position, glm::vec3(1.f, 0.f, 0.f));

	// Load the lemur model. Create a lemur object. Set its properties
	// Mesh downloaded from https://opengameart.org/content/lemur
	engine::ref<engine::model> lemur_model =
		engine::model::create("assets/models/static/lemur.obj");
	engine::game_object_properties lemur_props;
	lemur_props.meshes = lemur_model->meshes();
	lemur_props.textures = lemur_model->textures();
	float lemur_scale = 1.f / glm::max(lemur_model->size().x, glm::max(lemur_model->size().y,
		lemur_model->size().z));
	lemur_props.position = { 15.f,0.5f,10.f };
	lemur_props.scale = glm::vec3(lemur_scale);
	lemur_props.bounding_shape = lemur_model->size() / 2.f;
	lemur_props.type = 0;
	m_lemur = engine::game_object::create(lemur_props);
	m_lemur->set_offset(lemur_model->offset());
	m_lemur_box.set_box(lemur_props.bounding_shape.x * 2.f * lemur_scale,
		lemur_props.bounding_shape.y * 2.f * lemur_scale, lemur_props.bounding_shape.z * 2.f *
		lemur_scale, lemur_props.position - glm::vec3(0.f, m_lemur->offset().y, 0.f) * m_lemur->scale());
	m_lemur = engine::game_object::create(lemur_props);

	m_lemurNPC.initialise(m_lemur, lemur_props.position, glm::vec3(1.f, 0.f, 0.f));


	// Mesh downloaded from https://www.turbosquid.com/3d-models/3d-model-wooden-barrels-1488970
	engine::ref <engine::model> barrel_model = engine::model::create("assets/models/static/barrel_obj.obj");
	engine::game_object_properties barrel_props;
	barrel_props.meshes = barrel_model->meshes();
	barrel_props.textures = barrel_model->textures();
	float barrel_scale = 1.f / glm::max(barrel_model->size().x , glm::max(barrel_model->size().y, barrel_model->size().z));
	barrel_props.position = { 3.f,2.0f, 1.f };
	barrel_props.scale = glm::vec3(barrel_scale);
	barrel_props.bounding_shape = barrel_model->size()/2.f * (glm::vec3(2.f,1.f,2.f));
	barrel_props.type = 0;
	barrel_props.rotation_amount = glm::half_pi<float>();
	barrel_props.rotation_axis = glm::vec3(1.f, 0, 0);
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

	// Mesh downloaded from https://opengameart.org/content/bolt
	engine::ref <engine::model> bolt_shape = engine::model::create("assets/models/static/speed2.obj");
	engine::game_object_properties bolt_props;
	bolt_props.position = { 1.f,0.5f, -1.f };
	bolt_props.scale = { .5f,.5f,.5f };
	bolt_props.meshes = bolt_shape->meshes();
	bolt_props.textures = bolt_shape->textures();
	bolt_props.type = 0;
	bolt_props.bounding_shape = bolt_shape->size();
	bolt_props.restitution = 0.85f;
	bolt_props.mass = 0.45f;
	bolt_props.rolling_friction = 0.1f;
	m_lightningPickup = engine::game_object::create(bolt_props);;

	engine::ref<engine::sphere> torchLight_shape = engine::sphere::create(10, 20, 0.1f);
	engine::game_object_properties torchLight_props;
	torchLight_props.position = { 0.f, 5.f, -5.f };
	torchLight_props.meshes = { torchLight_shape->mesh() };
	torchLight_props.type = 1;
	torchLight_props.restitution = 0.85f;
	torchLight_props.bounding_shape = glm::vec3(0.1f);
	torchLight_props.mass = 0.45f;
	torchLight_props.scale = glm::vec3(0.1f);
	torchLight_props.rolling_friction = 0.1f;

	m_torchLight = engine::game_object::create(torchLight_props);

	//model downloaded from https://free3d.com/3d-model/grenade-443268.html
	engine::ref <engine::model> spell_shape = engine::model::create("assets/models/static/Grenade2.obj");
	engine::game_object_properties spell_props;
	spell_props.position = { 0.f, 20.f, -5.f };
	spell_props.meshes = spell_shape->meshes();
	spell_props.textures = spell_shape->textures();
	spell_props.type = 1;
	spell_props.bounding_shape = spell_shape->size();
	spell_props.restitution = 0.85f;
	spell_props.mass = 0.45f;
	spell_props.rolling_friction = 0.1f;
	m_spell = engine::game_object::create(spell_props);

	

	//model downloaded from https://free3d.com/3d-model/grenade-443268.html
	engine::ref <engine::model> grenade_shape = engine::model::create("assets/models/static/Grenade2.obj");
	engine::game_object_properties grenade_props;
	grenade_props.position = { 0.f,2.f, -13.f };
	grenade_props.meshes = grenade_shape->meshes();
	grenade_props.textures = grenade_shape->textures();
	grenade_props.type = 0;
	grenade_props.bounding_shape = grenade_shape->size();
	grenade_props.restitution = 0.85f;
	grenade_props.mass = 0.45f;
	grenade_props.rolling_friction = 0.1f;
	m_grenadePickup = engine::game_object::create(grenade_props);

	// Set up the sphere that is to be used for the light sources, mostly useful for debugging
	float radius = 0.1f;
	engine::ref<engine::sphere>sphere_shape = engine::sphere::create(10, 20, radius);
	engine::game_object_properties sphere_props;
	sphere_props.position = { 0.f, 5.f, -5.f };
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.type = 1;
	sphere_props.bounding_shape = glm::vec3(radius);
	sphere_props.restitution = 0.92f;
	sphere_props.mass = 0.01f;
	m_ball = engine::game_object::create(sphere_props);

	// Create and set up the potion primitive
	engine::ref<engine::potion> potion_shape =
		engine::potion::create();
	engine::game_object_properties potion_props;
	potion_props.position = { -1.f, .5f, -6.f };
	engine::ref<engine::texture_2d> potion_texture =
		engine::texture_2d::create("assets/textures/healthTex.png", true);
	potion_props.textures = { potion_texture };
	potion_props.meshes = { potion_shape->mesh() };
	potion_props.scale = glm::vec3(.1f, .1f, .1f);
	potion_props.rotation_amount = glm::pi<float>() / 4;
	potion_props.rotation_axis = glm::vec3(0, 1.f, 0);
	potion_props.type = 1;
	potion_props.bounding_shape = glm::vec3(.5f);
	potion_props.restitution = 0.92f;
	potion_props.mass = 1.0f;
	m_potion = engine::game_object::create(potion_props);

	// Create and set up the potion primitive
	engine::ref<engine::table> table_shape =
		engine::table::create();
	engine::game_object_properties table_props;
	table_props.position = { 2.f, .5f, -10.f };
	engine::ref<engine::texture_2d> table_texture =
		engine::texture_2d::create("assets/textures/darkWood.jpg", true);
	table_props.textures = { table_texture };
	table_props.meshes = { table_shape->mesh() };
	table_props.type = 1;
	table_props.bounding_shape = glm::vec3(.5f);
	table_props.restitution = 0.92f;
	table_props.mass = 1.0f;
	table_props.scale = glm::vec3(0.1f);
	m_table = engine::game_object::create(table_props);

	// Create and set up the torch primitive
	engine::ref<engine::torch> torch_shape =
		engine::torch::create();
	engine::game_object_properties torch_props;
	torch_props.position = { 0.f, 0.f, 0.f };
	engine::ref<engine::texture_2d> torch_texture =
		engine::texture_2d::create("assets/textures/stone.jpg", true);
	torch_props.textures = { torch_texture };
	torch_props.meshes = { torch_shape->mesh() };
	m_torch = engine::game_object::create(torch_props);


	m_game_objects.push_back(m_terrain);
	m_game_objects.push_back(m_intro);
	m_game_objects.push_back(m_options);

	m_game_objects.push_back(m_westEntranceWall);
	m_game_objects.push_back(m_eastEntranceWall);
	m_game_objects.push_back(m_topEntranceWall);
	m_game_objects.push_back(m_topMainWall);
	m_game_objects.push_back(m_southMainWall);
	//m_game_objects.push_back(m_southInnerWall);
	m_game_objects.push_back(m_southAngledWall);
	m_game_objects.push_back(m_southEastWall);
	m_game_objects.push_back(m_mainEastWall);
	m_game_objects.push_back(m_mainNorthWall);
	m_game_objects.push_back(m_mainWestWall);
	m_game_objects.push_back(m_splitterWall);

	m_game_objects.push_back(m_barrel);
	m_game_objects.push_back(m_sword);

	m_game_objects.push_back(m_lightningPickup);
	m_game_objects.push_back(m_grenadePickup);

	m_game_objects.push_back(m_mannequin);

	m_physics_manager = engine::bullet_manager::create(m_game_objects);
	m_text_manager = engine::text_manager::create();

	m_skinned_mesh->switch_animation(1);

	m_cross_fade_healing = cross_fade::create("assets/textures/green.bmp", 1.0f, 1.6f, 0.9f);
	m_cross_fade_damage = cross_fade::create("assets/textures/red.bmp", 1.0f, 1.6f, 0.9f);

	m_billboard = billboard::create("assets/textures/Explosion.tga", 4, 5, 16);

	//texture downloaded from https://www.pngitem.com/middle/iRiwhmT_pixel-heart-png-transparent-png/
	m_hud_hundred = hud::create("assets/textures/heart100.png", 0.2f, 0.2f);
	m_hud_eighty = hud::create("assets/textures/heart80.png", 0.2f, 0.2f);
	m_hud_sixty = hud::create("assets/textures/heart60.png", 0.2f, 0.2f);
	m_hud_forty = hud::create("assets/textures/heart40.png", 0.2f, 0.2f);
	m_hud_twenty = hud::create("assets/textures/heart20.png", 0.2f, 0.2f);
	m_hud_zero = hud::create("assets/textures/heart0.png", 0.2f, 0.2f);
	// All of the images are placed in a vector. As hp goes down, images get popped off and .back() is used
	// as the image on display
	m_heart_vector = { m_hud_zero, m_hud_twenty, m_hud_forty, m_hud_sixty, m_hud_eighty, m_hud_hundred };
	

	m_arcane_blast.initialise(m_spell);

	for (uint32_t i = 0; i < 3; i++) {
		m_lightning_bolts.push_back(lightning_bolt::create(glm::vec3(-4.f, 1.f, -4.85f), glm::vec3(0.f, 0.f, 1.f), 2.0f));
	}

}

example_layer::~example_layer() {}

void example_layer::on_update(const engine::timestep& time_step) 
{
	glm::vec3 enemy_pos = m_enemy.object()->position();

	m_gameOverTime += time_step;

	enemy_attackTime += time_step;
	lightningTime += time_step;
	tStep = time_step;

	if (m_player.getHealth() < 0)
	{
		m_gameStart = false;
		m_player.gameStart();
		m_gameOver = true;

	}

	checkHP();

	if (m_enemy.getState() == enemy_state::attacking  && enemy_attackTime >= enemy_attackCD && m_gameStart == true)
	{
		enemyAttack();
		enemy_attackTime = 0;
	}

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

	if (mimicAlive == true)
	{
		m_mimicNPC.on_update(time_step, m_player.object()->position());
	}

	//Update all of the bounding boxes in the scene
	m_cow_box.on_update(m_cow->position() - glm::vec3(0.f, m_cow->offset().y, 0.f)
		* m_cow->scale(), m_cow->rotation_amount(), m_cow->rotation_axis());

	m_mimic_box.on_update(m_mimic->position() - glm::vec3(0.f, m_mimic->offset().y, 0.f)
		* m_mimic->scale(), m_mimic->rotation_amount(), m_mimic->rotation_axis());

	m_lemur_box.on_update(m_lemur->position() - glm::vec3(0.f, m_lemur->offset().y, 0.f)
		* m_lemur->scale(), m_lemur->rotation_amount(), m_lemur->rotation_axis());

	m_barrel_box.on_update(m_barrel->position() - glm::vec3(0.f, m_barrel->offset().y, 0.f)
		* m_barrel->scale(), m_barrel->rotation_amount(), m_barrel->rotation_axis());

	m_lemurNPC.on_update(time_step, m_player.object()->position());

	m_billboard->on_update(time_step);

	m_arcane_blast.on_update(time_step);

	m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

	m_audio_manager->update_with_camera(m_3d_camera);

	m_cross_fade_healing->on_update(time_step);
	m_cross_fade_damage->on_update(time_step);

	//resets the position of the horror and player so their bounding boxes dont intersect
	if (m_cow_box.collision(m_player.getBox()))
	{
		m_enemy.object()->set_position(enemy_pos);
	}

	// Catching the lemur will give the player a speed increase and also allow them to carry out a
	// 180 degree turn with the G key.
	if (m_lemur_box.collision(m_player.getBox()))
	{
		m_player.increaseSpeed();
		lemurCaught = false;
		m_audio_manager->play("speedup");
		m_lemur->set_position(glm::vec3(20.f,-20.f,20.f));
		m_lemur_box.setPosition(glm::vec3(20.f, -20.f, 20.f));
		m_player.buffed();
		
	}

	// Distance-based collision between the mimic and grenade
	// The trick is that if the mimic is aware while being killed, it will not drop the grenade which is
	// key for getting to Level 2.
	if (glm::length(m_spell->position() - m_mimic->position()) < 2.f)
	{
		
		m_billboard->activate(glm::vec3(m_mimic->position()), 4.f, 4.f);
		mimicAlive = false;
		if (m_mimicNPC.getState() != state::aware)
		{
			grenadeSpawn = m_mimic->position();
			spawnGrenade = true;
			unawareMimicKilled = true;
		}
		else
		{
			unawareMimicKilled = false;
		}

		m_mimic->set_position(glm::vec3(20.f, -40.f, 20.f));
		m_mimic_box.setPosition(glm::vec3(20.f, -40.f, 20.f));

	}

	//running on_update for the lighning bolts
	for (uint32_t i = 0; i < m_lightning_bolts.size(); i++) {
		m_lightning_bolts.at(i)->on_update(time_step);
	}

	// distance-based collision for picking up the grenade powerup
	if (glm::length(m_player.position() - m_grenadePickup->position()) < 3.f && unawareMimicKilled == true)
	{
		blastAvailable = true;
		unawareMimicKilled = false;
		spawnGrenade = false;
		m_player.setPosition(glm::vec3(7.f, 0.5f, -16.f));
	}

	// distance-based collision for picking up the health potion
	if (abs(glm::length(m_player.position() - m_potion->position())) < 2.f && spawnPotion == true)
	{
		healingAvailable = true;
		spawnPotion = false;
	}

	// distance-based collision for picking up the lightning powerup
	if (glm::length(m_player.position() - m_lightningPickup->position()) < 1.f && lightningPickup == true)
	{
		lightningAvailable = true;
		lightningPickup = false;
		m_audio_manager->play("pickup");
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

	//house walls
	engine::renderer::submit(mesh_shader, m_westEntranceWall);
	engine::renderer::submit(mesh_shader, m_eastEntranceWall);
	engine::renderer::submit(mesh_shader, m_topEntranceWall);
	engine::renderer::submit(mesh_shader, m_topMainWall);
	engine::renderer::submit(mesh_shader, m_southMainWall);
	engine::renderer::submit(mesh_shader, m_southInnerWall);
	engine::renderer::submit(mesh_shader, m_southAngledWall);
	engine::renderer::submit(mesh_shader, m_southEastWall);
	engine::renderer::submit(mesh_shader, m_mainEastWall);
	engine::renderer::submit(mesh_shader, m_mainNorthWall);
	engine::renderer::submit(mesh_shader, m_mainWestWall);
	engine::renderer::submit(mesh_shader, m_splitterWall);

	// Press 8 to show the bounding boxes 
	if (drawBoundingBoxes == true)
	{
		m_barrel_box.on_render(2.5f, 0.f, 0.f, mesh_shader);

		m_player.getBox().on_render(2.5f, 0.f, 0.f, mesh_shader);

		m_cow_box.on_render(2.5f, 0.f, 0.f, mesh_shader);

		m_mimic_box.on_render(2.5f, 0.f, 0.f, mesh_shader);

		m_lemur_box.on_render(2.5f, 0.f, 0.f, mesh_shader);
	}
	


	engine::renderer::submit(mesh_shader, m_barrel);

	engine::renderer::submit(mesh_shader, m_intro);
	engine::renderer::submit(mesh_shader, m_options);

	if (spawnPotion == true)
	{
		glm::mat4 potion_transform(1.0f);
		potion_transform = glm::translate(potion_transform, m_potion->position());
		potion_transform = glm::rotate(potion_transform, m_potion->rotation_amount(), m_potion->rotation_axis());
		potion_transform = glm::scale(potion_transform, m_potion->scale());
		engine::renderer::submit(mesh_shader, potion_transform, m_potion);
	}

	// Spawn the grenade (key to next level) if the mimic was unaware when killed
	if (unawareMimicKilled == true && spawnGrenade == true)
	{ 
		glm::mat4 grenade_transform(1.0f);
		grenade_transform = glm::translate(grenade_transform, grenadeSpawn);
		grenade_transform = glm::rotate(grenade_transform, m_grenadePickup->rotation_amount(), m_grenadePickup->rotation_axis());
		grenade_transform = glm::scale(grenade_transform, m_grenadePickup->scale());
		engine::renderer::submit(mesh_shader, grenade_transform, m_grenadePickup);
	}

	glm::mat4 table_transform(1.0f);
	table_transform = glm::translate(table_transform, m_table->position());
	table_transform = glm::rotate(table_transform, m_table->rotation_amount(), m_table->rotation_axis());
	table_transform = glm::scale(table_transform, m_table->scale());
	engine::renderer::submit(mesh_shader, table_transform, m_table);

	// Render the cow if it is not dead, otherwise declare game over
	if (m_enemy.getHealth() > 0)
	{
		glm::mat4 cow_transform(1.0f);
		cow_transform = glm::translate(cow_transform, m_cow->position());
		cow_transform = glm::rotate(cow_transform, m_cow->rotation_amount(), m_cow->rotation_axis());
		cow_transform = glm::scale(cow_transform, m_cow->scale());
		engine::renderer::submit(mesh_shader, cow_transform, m_cow);
	}
	else
	{
		m_gameStart = false;
		m_player.gameStart();
		m_gameOver = true;
	}
	
	// Render the mimic if it has not been destroyed
	if (mimicAlive == true)
	{
		glm::mat4 mimic_transform(1.0f);
		mimic_transform = glm::translate(mimic_transform, m_mimic->position());
		mimic_transform = glm::rotate(mimic_transform, m_mimic->rotation_amount(), m_mimic->rotation_axis());
		mimic_transform = glm::scale(mimic_transform, m_mimic->scale());
		engine::renderer::submit(mesh_shader, mimic_transform, m_mimic);
	}

	// Render the lemur if it has not been caught
	if (lemurCaught == true)
	{
		glm::mat4 lemur_transform(1.0f);
		lemur_transform = glm::translate(lemur_transform, m_lemur->position());
		lemur_transform = glm::rotate(lemur_transform, m_lemur->rotation_amount(), m_lemur->rotation_axis());
		lemur_transform = glm::scale(lemur_transform, m_lemur->scale());
		engine::renderer::submit(mesh_shader, lemur_transform, m_lemur);
	}

	// Table primitives
	glm::mat4 flipped_table_transform(1.0f);
	flipped_table_transform = glm::translate(flipped_table_transform, glm::vec3(3.8f,0.5f,-11.f));
	//flipped_table_transform = glm::rotate(flipped_table_transform, glm::pi<float>(), glm::vec3(1.f,0,0));
	flipped_table_transform = glm::rotate(flipped_table_transform, glm::pi<float>()/4, glm::vec3(0, 1.f, 0));
	flipped_table_transform = glm::scale(flipped_table_transform, glm::vec3(.01f));
	engine::renderer::submit(mesh_shader, flipped_table_transform, m_table);

	// Table primitives
	glm::mat4 second_flipped_table_transform(1.0f);
	second_flipped_table_transform = glm::translate(second_flipped_table_transform, glm::vec3(9.5f, 1.25f, -16.f));
	second_flipped_table_transform = glm::rotate(second_flipped_table_transform, glm::pi<float>(), glm::vec3(1.f, 0, 0));
	second_flipped_table_transform = glm::rotate(second_flipped_table_transform, glm::pi<float>() / 6, glm::vec3(0, 1.f, 0));
	second_flipped_table_transform = glm::scale(second_flipped_table_transform, glm::vec3(.03f,0.01f,0.04f));
	engine::renderer::submit(mesh_shader, second_flipped_table_transform, m_table);

	m_arcane_blast.on_render(mesh_shader);

	m_material->submit(mesh_shader);

	std::vector<glm::vec3> barrelStack{
		glm::vec3(0.f, 0.9f, -18.8f),
		glm::vec3(-1.f, 0.9f, -18.8f),
		glm::vec3(-0.5f, 1.5f, -18.8f),
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
	z_barrel_transform = glm::translate(z_barrel_transform, glm::vec3(1.5f, 0.9f, -18.5f));
	z_barrel_transform = glm::rotate(z_barrel_transform, glm::half_pi<float>(), glm::vec3(0.f, 0.f, 1.f));
	z_barrel_transform = glm::scale(z_barrel_transform, glm::vec3(.01f));
	engine::renderer::submit(mesh_shader, z_barrel_transform, m_barrel);

	glm::mat4 y_barrel_transform(1.0f);
	y_barrel_transform = glm::translate(y_barrel_transform, glm::vec3(2.f, 0.5f, -18.5f));
	y_barrel_transform = glm::scale(y_barrel_transform, glm::vec3(.01f));
	engine::renderer::submit(mesh_shader, y_barrel_transform, m_barrel);

	glm::mat4 yy_barrel_transform(1.0f);
	yy_barrel_transform = glm::translate(yy_barrel_transform, glm::vec3(2.f, 1.5f, -18.5f));
	yy_barrel_transform = glm::scale(yy_barrel_transform, glm::vec3(.01f));
	engine::renderer::submit(mesh_shader, yy_barrel_transform, m_barrel);

	// Set up the bolt transformations
	if (lightningPickup == true)
	{
		glm::mat4 bolt_transform(1.0f);
		bolt_transform = glm::translate(bolt_transform, m_lightningPickup->position());
		bolt_transform = glm::rotate(bolt_transform, boltRotation, glm::vec3(0,1.f,0));
		bolt_transform = glm::scale(bolt_transform, m_lightningPickup->scale());
		engine::renderer::submit(mesh_shader, bolt_transform, m_lightningPickup);
		boltRotation += glm::pi<float>() / 180;
	}

	glm::mat4 sword_transform(1.0f);
	sword_transform = glm::translate(sword_transform, m_player.equipment_position());
	sword_transform = glm::rotate(sword_transform, glm::atan(m_3d_camera.front_vector().x, m_3d_camera.front_vector().z), glm::vec3(0.f, 1.f, 0.f));
	sword_transform = glm::scale(sword_transform, glm::vec3(.1f));
	engine::renderer::submit(mesh_shader,sword_transform, m_sword);

	glm::mat4 torch_two_transform(1.0f);
	torch_two_transform = glm::translate(torch_two_transform, glm::vec3( -1.7f, 2.f,-10.f));
	torch_two_transform = glm::scale(torch_two_transform, glm::vec3(0.15f));
	torch_two_transform = glm::rotate(torch_two_transform, glm::half_pi<float>(), glm::vec3(0,-1.f,0));
	torch_two_transform = glm::rotate(torch_two_transform, glm::half_pi<float>()/4, glm::vec3(.1f, 0, 0));
	engine::renderer::submit(mesh_shader, torch_two_transform, m_torch);

	glm::mat4 torch_three_transform(1.0f);
	torch_three_transform = glm::translate(torch_three_transform, glm::vec3(4.7f, 2.f, -15.f));
	torch_three_transform = glm::scale(torch_three_transform, glm::vec3(0.15f));
	torch_three_transform = glm::rotate(torch_three_transform, glm::half_pi<float>(), glm::vec3(0, 1.f, 0));
	torch_three_transform = glm::rotate(torch_three_transform, glm::half_pi<float>() / 4, glm::vec3(0, 0, .1f));
	engine::renderer::submit(mesh_shader, torch_three_transform, m_torch);

	glm::mat4 torch_four_transform(1.0f);
	torch_four_transform = glm::translate(torch_four_transform, glm::vec3(11.25f, 2.f, -5.85f));
	torch_four_transform = glm::scale(torch_four_transform, glm::vec3(0.15f));
	torch_four_transform = glm::rotate(torch_four_transform, glm::half_pi<float>() - glm::pi<float>()/9, glm::vec3(0, 1.f, 0));
	engine::renderer::submit(mesh_shader, torch_four_transform, m_torch);

	glm::mat4 torch_five_transform(1.0f);
	torch_five_transform = glm::translate(torch_five_transform, glm::vec3(9.f, 2.f, -18.7f));
	torch_five_transform = glm::scale(torch_five_transform, glm::vec3(0.15f));
	torch_five_transform = glm::rotate(torch_five_transform, glm::pi<float>(), glm::vec3(0, 1.f, 0));
	torch_five_transform = glm::rotate(torch_five_transform, glm::pi<float>() / 3, glm::vec3(0, 0, .1f));
	engine::renderer::submit(mesh_shader, torch_five_transform, m_torch);

	// Transforms the red ball that is the light source of the torch, other broken torches do not have this
	glm::mat4 torchLight_five_transform(1.0f);
	torchLight_five_transform = glm::translate(torchLight_five_transform, glm::vec3(10.82f, 3.0f, -5.97f));
	torchLight_five_transform = glm::scale(torchLight_five_transform, glm::vec3(1.4f, 1.4f, 1.4f));
	engine::renderer::submit(mesh_shader, torchLight_five_transform, m_torchLight);

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
		set_uniform("gNumPointLights", (int)num_point_lights);
	m_pointLight_three.submit(mesh_shader, 0);

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
		set_uniform("gNumSpotLights", (int)num_spot_lights);
	m_intro_spotLight.submit(mesh_shader, 0);

	m_mannequin_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_player.object());

	// render all of the balls for every light source
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
		set_uniform("lighting_on", false);

	m_lightsource_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_ball->meshes().at(0),
		glm::translate(glm::mat4(1.f), m_pointLight_three.Position));

	m_lightsource_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_ball->meshes().at(0),
		glm::translate(glm::mat4(1.f), m_intro_spotLight.Position));

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
		set_uniform("lighting_on", true);

	for (uint32_t i = 0; i < m_lightning_bolts.size(); i++) {
		m_lightning_bolts.at(i)->on_render(mesh_shader);
	}

    engine::renderer::end_scene();

	if (m_gameStart)
	{
		//display the heart monitor on the HUD
		engine::renderer::begin_scene(m_2d_camera, mesh_shader);

		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
			set_uniform("lighting_on", false);
		m_heart_vector.back()->on_render(mesh_shader);
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
			set_uniform("lighting_on", true);

		engine::renderer::end_scene();
	}
	// Render text
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	if (m_gameStart)
	{
		m_text_manager->render_text(text_shader, "Horror Health: " + std::to_string(m_enemy.getHealth()),
			10.f,
			(float)engine::application::window().height() - 75.f,
			0.5f,
			glm::vec4(1.f, 0.5f, 0.f, 1.f));
	}

	// blink some text saying "GAME OVER" when either the player of the cow dies
	if (m_gameOver && m_gameOverTime <= m_gameOverCD)
	{
		m_text_manager->render_text(text_shader, "GAME OVER" ,
			(float)engine::application::window().width()/2 - 300,
			(float)engine::application::window().height()/2,
			2.f,
			glm::vec4(0.5f, 0.1f, 0.7f, 1.f));
	}
	else if (m_gameOver && m_gameOverTime >= 2 * m_gameOverCD)
	{
		m_gameOverTime = 0.f;
	}

	//Render the explosion and damage crossfade effects
	engine::renderer::begin_scene(m_2d_camera, mesh_shader);
	m_cross_fade_healing->on_render(mesh_shader);
	engine::renderer::end_scene();

	engine::renderer::begin_scene(m_2d_camera, mesh_shader);
	m_cross_fade_damage->on_render(mesh_shader);
	engine::renderer::end_scene();

	engine::renderer::begin_scene(m_3d_camera, mesh_shader);
	m_billboard->on_render(m_3d_camera, mesh_shader);
	engine::renderer::end_scene();

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
			//background sounds consist of three tracks stacked on top of each other.
			m_gameStart = true;
			m_3d_camera.position(glm::vec3(0,1.f,0));
			m_audio_manager->pause("music");
			m_audio_manager->play("birds");
			m_audio_manager->play("ominous");
			m_audio_manager->play("rain");
		}
		if (e.key_code() == engine::key_codes::KEY_RIGHT && !m_gameStart && !m_display_options)
		{
			//Moves the spotlight based on what menu the player is
			m_display_options = true;
			m_3d_camera.position(glm::vec3(21.f, -20.2f, 13.0));
			m_intro_spotLight.Position += glm::vec3(21.f,0,0);
		}
		if (e.key_code() == engine::key_codes::KEY_LEFT && !m_gameStart && m_display_options)
		{
			//Moves the spotlight based on what menu the player is
			m_display_options = false;
			m_3d_camera.position(glm::vec3(0, -20.f, 13.0));
			m_intro_spotLight.Position += glm::vec3(-21.f, 0, 0);
		}
		//if (e.key_code() == engine::key_codes::KEY_J)
		//{
		//	//toggle to freecam for debugging
		//	freeCam = !freeCam;
		//}
		if (e.key_code() == engine::key_codes::KEY_1 && blastAvailable && m_gameStart)
		{
			//throws a grenade, a player can only have 1 grenade equipped at a time
			m_audio_manager->play("throw");
			m_arcane_blast.throw_spell(m_3d_camera, 100.0f);
			blastAvailable = false;
		}
		if (e.key_code() == engine::key_codes::KEY_3 && healingAvailable && m_gameStart)
		{
			//when the potion is consumed, the player is healed and a green screen appears
			m_cross_fade_healing->activate();
			m_player.heal();
			healingAvailable = false;
			spawnPotion = false;
			m_audio_manager->play("healing");
			/*float hp = m_player.getHealth();
			if		(hp >= 80) {m_heart_vector.push_back(m_hud_eighty);		heart_pops = 0;}
			else if (hp >= 60) {m_heart_vector.push_back(m_hud_sixty);		heart_pops = 1;}
			else if (hp >= 40) {m_heart_vector.push_back(m_hud_forty);		heart_pops = 2;}
			else if (hp >= 20) {m_heart_vector.push_back(m_hud_twenty);		heart_pops = 3;}*/
		}
		if (e.key_code() == engine::key_codes::KEY_2 && lightningAvailable == true && m_gameStart)
		{
			//shoots out lightning on a 3 second cooldown. Also plays a sound effect
			if (lightningTime >= lightningCD)
			{
				m_audio_manager->play("electricity");
				for (uint32_t i = 0; i < m_lightning_bolts.size(); i++) {
					m_lightning_bolts.at(i)->activate(m_player.position(), m_player.getForward());
				}
				// Deals damage to the main enemy if within the radius of the electricity
				if (glm::length(m_player.position() - m_cow->position()) < 4.f)
				{
					m_enemy.takeDamage(10);
				}
				//begin the cooldown (variable incremented by time_step in on_update)
				lightningTime = 0;
			}
		}
		if (e.key_code() == engine::key_codes::KEY_8)
		{
			if (drawBoundingBoxes == true) drawBoundingBoxes = false;
			else drawBoundingBoxes = true;
		}
    } 
}

void example_layer::check_bounce()
{
	if (m_prev_sphere_y_vel < 0.1f && m_spell->velocity().y > 0.1f) 
	m_prev_sphere_y_vel = m_game_objects.at(1)->velocity().y;
}

//Used to reset the position of the wall prefab when building the house in lines 162-233.
void example_layer::resetPropPosition(engine::game_object_properties props)
{
	props.position = glm::vec3(wallLength, wallDepth, wallWidth);
}

//Update the heart counter based on the player's HP
// Do it using an index that is increased and decreased 
void example_layer::checkHP()
{
	int hp = m_player.getHealth();
	if (hp <= 80 && heart_pops == 0)	popHeart();
	if (hp <= 60 && heart_pops == 1)	popHeart();
	if (hp <= 40 && heart_pops == 2)	popHeart();
	if (hp <= 20 && heart_pops == 3)	popHeart();
	if (hp <= 0  && heart_pops == 4)	popHeart();
}

//Quick function to avoid repetition and make the above code cleaner
void example_layer::popHeart() {
	m_heart_vector.pop_back();
	++heart_pops;
}

//Enemy's attack whenever in range. Should be move into enemy.
void example_layer::enemyAttack()
{
	m_player.receivedHit(20);
	m_cross_fade_damage->activate();
}
