#pragma once
#include <vector>
#include <blocks.h>

struct GameMap
{
	int width{ 0 };
	int height{ 0 };

	std::vector<Block> foreGroundData;
	std::vector<Block> backGroundData;

	void Create(int width, int height);

	Block &GetBlockUnsafe(int x, int y);

	Block* GetBlockSafe(int x, int y);

	Block& GetBackgroundBlockUnsafe(int x, int y);

	Block* GetBackgroundBlockSafe(int x, int y);
};