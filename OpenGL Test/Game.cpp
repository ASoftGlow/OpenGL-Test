#include "game.h"
#include "resources.hpp"
#include "platform_paths.h"

TerrainRenderer* renderer;


Game::~Game()
{
	delete renderer;
	delete terrain;
}


void Game::init()
{
	// load shaders
	Shader* terrain_shader = Resources::loadShader(
		platform_paths::getStaticAppFolder() / "assets/shaders/terrain.vert",
		platform_paths::getStaticAppFolder() / "assets/shaders/terrain.frag", "terrain");

	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, (float)window_width,
		(float)window_height, 0.0f, -1.0f, 1.0f);
	terrain_shader->use();
	terrain_shader->setInt("image", 0);
	terrain_shader->setMatrix4("projection", projection);

	// load textures
	Resources::loadTexture(platform_paths::getStaticAppFolder() / "assets/atlas2.png", true, "terrain_atlas", false);
	terrain_shader->setInt("terrain_atlas", 0);
	Resources::loadTexture(platform_paths::getStaticAppFolder() / "assets/foliage_mask.png", true, "foliage_mask", false);
	terrain_shader->setInt("foliage_mask", 1);
	Resources::loadTexture(platform_paths::getStaticAppFolder() / "assets/icons.png", true, "icons", false);

	// random_gen terrain
	terrain = new Terrain{
		&SaveManager::current.chunk_size,
		&SaveManager::current.chunks
	};

	// create renderers
	renderer = new TerrainRenderer(*terrain_shader, terrain);
}


Chunk* Game::getCurrentChunk()
{
	return terrain->getChunk(
		-(int)floor(pan_x / (*terrain->chunk_size * 8) + 0.5),
		(int)floor(pan_y / (*terrain->chunk_size * 8) + 0.5)
	);
}


void Game::loadVisableChunks()
{
	const int current_chunk_x = -(int)floor(pan_x / (*terrain->chunk_size * 8) + 0.5);
	const int current_chunk_y = (int)floor(pan_y / (*terrain->chunk_size * 8) + 0.5);

	const float offset_from_chunk_x = (float)(pan_x / (*terrain->chunk_size * 8) + 0.5) + current_chunk_x;
	const float offset_from_chunk_y = (float)(pan_y / (*terrain->chunk_size * 8) + 0.5) - current_chunk_y;

	const int view_chunk_length = (int)ceil((float)window_width / 8 / *terrain->chunk_size / zoom);// + (offset_from_chunk_x >= (*terrain->chunk_size * 8));
	const int view_chunk_height = (int)ceil((float)window_height / 8 / *terrain->chunk_size / zoom);// +();

	std::vector<std::pair<int, int>> chunksToGenerate;
	//printf("\rzoom: %f chunk: (%f,%f) ", zoom, offset_from_chunk_x, offset_from_chunk_y);
	// Load existing chunks 
	for (int j = 0; j < view_chunk_height + (view_chunk_height % 2 == 0); j++)
	{
		if (j == -1) std::cout << j;
		for (int k = 0; k < view_chunk_length + (view_chunk_length % 2 == 0); k++)
		{
			int y = current_chunk_y - (int)(view_chunk_height / 2) + j;
			int x = current_chunk_x - (int)(view_chunk_length / 2) + k;

			Chunk* chunk = terrain->getChunk(x, y);
			if (chunk == nullptr)
			{
				chunksToGenerate.push_back(std::pair<int, int>(x, y));
			}
		}
	}

	// Create unexisting chunks
	/*for (auto& [x, y] : chunksToGenerate)
	{
		printf("Generating chunk (%i,%i)\n", x, y);
		terrain->generateChunk(x, y);
	}*/
	terrain->generateChunks(chunksToGenerate);
}


void Game::render()
{
	// terrain
	renderer->drawTerrain(
		*Resources::getTexture("terrain_atlas"),
		*Resources::getTexture("foliage_mask"),
		this->pan_x * zoom + (float)window_width / 2,
		this->pan_y * zoom + (float)window_height / 2,
		this->zoom * 8,
		0.0f);

	// sprites
	//renderer->DrawSprite(t, glm::vec2(200.0f, 200.0f), 200.0f, 45.0f);
}


void Game::update()
{

	//std::cout << "x: " << this->x << "\t(" << x << ")\ty: " << this->y << "\t(" << y << ")" << std::endl;
	//terrain->chunks->clear();
	//loadVisableChunks();
	//renderer->updateVBO();

}

void Game::save()
{
	if (SaveManager::save())
		Logger::info("Saved");
	else
		Logger::error("Failed to save");
}

void Game::save(int id)
{
	SaveManager::current.id = id;
	save();
}

void Game::load(int id)
{
	if (SaveManager::load(id))
	{
		terrain->init();
		loadVisableChunks();
		renderer->updateVBO();
		Logger::info("Loaded save id: ");
		printf("%i", id);
	}
	else
		Logger::error("Failed to load");
}

void Game::create()
{
	SaveManager::newSave("My World");
	terrain->init();
	// First chunks
	//terrain->generateChunk(0, 0);
	/*terrain->generateChunk(1, 0);
	terrain->generateChunk(0, 1);
	terrain->generateChunk(1, 1);*/
	loadVisableChunks();

	renderer->updateVBO();
}

void Game::quit()
{
	SaveManager::current = Save{};
	renderer->updateVBO();
}

void Game::importSave(const char* path)
{
	if (SaveManager::importSave(path))
		Logger::info((std::string("Imported save from ") + path).c_str());
	else
		Logger::error((std::string("Failed to imported save from ") + path).c_str());
}

void Game::exportSave(const char* path)
{
	if (SaveManager::exportSave(path))
		Logger::info((std::string("Exported save to ") + path).c_str());
	else
		Logger::error((std::string("Failed to export save to ") + path).c_str());
}

