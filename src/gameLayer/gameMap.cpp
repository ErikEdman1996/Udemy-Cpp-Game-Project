#include <gameMap.h>
#include <asserts.h>

void GameMap::Create(int width, int height)
{
	*this = {};		// Clear the map data and reset width and height to 0 before setting new values
	
	mapData.resize(width * height);
	
	this->width = width;

	this->height = height;

	for(auto &block : mapData)
	{
		block.type = Block::Type::Air;
	}
}

Block &GameMap::GetBlockUnsafe(int x, int y)
{
	permaAssertCommentDevelopement(mapData.size() == width * height, "Map data is not initialized");

	permaAssertCommentDevelopement(x >= 0 && y >= 0 && x <= width && y <= height, "GetBlockSafe out of bounds error");

	return mapData[x + y * width];
}

Block *GameMap::GetBlockSafe(int x, int y)
{
	permaAssertCommentDevelopement(mapData.size() == width * height, "Map data is not initialized");

	if(x < 0 || y < 0 || x >= width || y >= height)
	{
		return nullptr;		//Out of bounds, return null pointer
	}

	return &mapData[x + y * width];
}

