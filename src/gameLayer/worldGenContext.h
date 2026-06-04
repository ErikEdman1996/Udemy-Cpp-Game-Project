#pragma once
#include <vector>
#include "randomStuff.h"
#include "gameMap.h"
#include "biomes.h"

struct WorldGenContext
{
	GameMap& map;

	int width{ 0 };
	int height{ 0 };
	int seed{ 0 };

	std::ranlux24_base rng{ 0 };

	std::vector<int> surfaceHeights;
	std::vector<int> stoneHeights;
	std::vector<BiomeRegion> biomeRegions;
};