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

	float terrainHeightMultiplier;
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
			Biome{ BiomeType::Forest, Block::Type::GrassBlock, Block::Type::Dirt, Block::Type::DirtWall, 150, 300, 1.0f };
	case BiomeType::Desert:
		return 
			Biome{ BiomeType::Desert, Block::Type::Sand, Block::Type::Sand, Block::Type::SandWall, 100, 200, 0.35f };
	case BiomeType::Snow:
		return 
			Biome{ BiomeType::Snow, Block::Type::Snow, Block::Type::Dirt, Block::Type::SnowWall, 100, 200, 0.8f };
	default:
		return 
			GetBiome(BiomeType::Forest);
	}
}