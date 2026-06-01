#pragma once
#include <vector>
#include <blocks.h>

struct GameMap
{
	int width{ 0 };
	int height{ 0 };

	std::vector<Block> mapData;

	void Create(int width, int height);

	Block &GetBlockUnsafe(int x, int y);

	Block* GetBlockSafe(int x, int y);
};