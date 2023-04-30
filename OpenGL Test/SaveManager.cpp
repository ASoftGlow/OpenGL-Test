#include "SaveManager.h"
#include <fstream>
#include <string>
#include <iostream>

SaveManager::SaveManager()
{

}

void SaveManager::load(unsigned id)
{
	std::string filePath = "\\saves\\" + std::to_string(id) + "\\game.data";

	std::ifstream file;
	file.open(filePath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		std::cout << "Failed to open file." << std::endl;
		return;
	}
	std::streampos size = file.tellg();
	char* memblock = new char[size];
	file.seekg(0, std::ios::beg);
	file.read(memblock, size);
	file.close();

	// process data
	const char version = memblock[0];

	switch (version)
	{
	case 1:
	default:
		const unsigned width = (unsigned)memblock[1];
		const unsigned height = (unsigned)memblock[2];

		std::vector<Tile> tileData;
		tileData.reserve(width * height);

		for (unsigned int i = 0; i < width * height; i++)
		{
			tileData.push_back(Tile{ (TileType)memblock[3 + i] });
		}
		/*delete terrain;
		terrain = new Terrain{ width, height };
		terrain->loadData(tileData);*/

		break;
	}

	delete[] memblock;
}
