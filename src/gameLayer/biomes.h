#pragma once
#include "blocks.h"

enum class BiomeType
{
	Forest,
	Desert,
	Snow
};

struct Biome
{
	BiomeType type;

	Block::Type surfaceBlock;
	Block::Type dirtBlock;
	Block::Type wallBlock;

	int minWidth;
	int maxWidth;

	int minSurfaceHeight;
	int maxSurfaceHeight;

	int minStoneDepth;
	int maxStoneDepth;

	int minDirectionTime;
	int maxDirectionTime;

	float heightChangeChance;
	int maxStepAmount;

	int stoneMinDirectionTime;
	int stoneMaxDirectionTime;
	float stoneDepthChangeChance;
	int stoneMaxStepAmount;
};

struct BiomeRegion
{
	BiomeType type;
	int startX;
	int endX;
};

inline Biome GetBiome(BiomeType type)
{
	switch (type)
	{
	case BiomeType::Forest:
		return 
			Biome{ 
			BiomeType::Forest, 
			Block::Type::GrassBlock, 
			Block::Type::Dirt, 
			Block::Type::DirtWall, 
			150, 300, 
			110, 160,
			3, 70,
			10, 40,
			0.25f,
			1,
			10,
			50,
			0.20f,
			1
		};

	case BiomeType::Desert:
		return 
			Biome{ 
			BiomeType::Desert, 
			Block::Type::Sand, 
			Block::Type::Sand, 
			Block::Type::SandWall, 
			100, 200, 
			130, 150,
			20, 50, 
			20, 70, 
			0.12f,
			1,
			20,
			80,
			0.10f,
			1
		};

	case BiomeType::Snow:
		return 
			Biome{ 
			BiomeType::Snow, 
			Block::Type::Snow, 
			Block::Type::Dirt, 
			Block::Type::SnowWall, 
			100, 200, 
			80, 140,
			0, 90,
			5, 30,
			0.35f,
			2,
			5,
			35,
			0.35f,
			2
		};

	default:
		return 
			GetBiome(BiomeType::Forest);
	}
}