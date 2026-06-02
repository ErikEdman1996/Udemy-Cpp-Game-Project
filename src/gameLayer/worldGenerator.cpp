#include "worldGenerator.h"
#include "randomStuff.h"
#include <raymath.h>

void GenerateWorld(GameMap& map, int seed)
{
	const int width{ 900 };
	const int height{ 500 };

	map.Create(width, height);

	auto unsignedSeed = static_cast<std::ranlux24_base::result_type>(seed);
	std::ranlux24_base rng{ unsignedSeed };

	int directionDirt = GetRandomInt(rng, -2, 2);
	int keepDirectionDirtTime = GetRandomInt(rng, 5, 50);

	int directionStone = GetRandomInt(rng, -2, 2);
	int keepDirectionStoneTime = GetRandomInt(rng, 5, 50);

	int dirtHeight = 70;
	int stoneHeight = 90;


	for (int x = 0; x < width; x++)
	{
		keepDirectionDirtTime--;

		if(keepDirectionDirtTime <= 0)
		{
			directionDirt = GetRandomInt(rng, -2, 2);
			keepDirectionDirtTime = GetRandomInt(rng, 5, 50);
		}

		if(directionDirt == -1)
		{
			if(GetRandomChance(rng, 0.25f))
			{
				dirtHeight--;
			}
		}
		else if(directionDirt == -2)
		{
			if (GetRandomChance(rng, 0.25f))
			{
				dirtHeight--;
			}
			if (GetRandomChance(rng, 0.25f))
			{
				dirtHeight--;
			}
		}
		else if(directionDirt == 1)
		{
			if (GetRandomChance(rng, 0.25f))
			{
				dirtHeight++;
			}
		}
		else if (directionDirt == 2)
		{
			if (GetRandomChance(rng, 0.25f))
			{
				dirtHeight++;
			}
			if (GetRandomChance(rng, 0.25f))
			{
				dirtHeight++;
			}
		}

		//Stone
		keepDirectionStoneTime--;

		if (keepDirectionStoneTime <= 0)
		{
			directionStone = GetRandomInt(rng, -2, 2);
			keepDirectionStoneTime = GetRandomInt(rng, 5, 50);
		}

		if (directionStone == -1)
		{
			if (GetRandomChance(rng, 0.25f))
			{
				stoneHeight--;
			}
		}
		else if (directionStone == -2)
		{
			if (GetRandomChance(rng, 0.25f))
			{
				stoneHeight--;
			}
			if (GetRandomChance(rng, 0.25f))
			{
				stoneHeight--;
			}
		}
		else if (directionStone == 1)
		{
			if (GetRandomChance(rng, 0.25f))
			{
				stoneHeight++;
			}
		}
		else if (directionStone == 2)
		{
			if (GetRandomChance(rng, 0.25f))
			{
				stoneHeight++;
			}
			if (GetRandomChance(rng, 0.25f))
			{
				stoneHeight++;
			}
		}

		if(stoneHeight < 60)
		{
			stoneHeight = 60;
		}

		if(stoneHeight > 120)
		{
			stoneHeight = 120;
		}
		
		for (int y = 0; y < height; y++)
		{
			Block block;

			if (y > dirtHeight)
			{
				block.type = Block::Type::Dirt;
			}
			if(y == dirtHeight)
			{
				block.type = Block::Type::GrassBlock;
			}
			if(y > stoneHeight)
			{
				block.type = Block::Type::Stone;
			}

			map.GetBlockUnsafe(x, y) = block;
		}
	}
}