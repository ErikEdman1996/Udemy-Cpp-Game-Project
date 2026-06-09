#include "worldGenerator.h"
#include "randomStuff.h"
#include <raymath.h>
#include <FastNoiseSIMD.h>
#include "biomes.h"
#include <vector>

#include "worldGenContext.h"
#include "perlinWorm.h"


void CarveCircle(WorldGenContext& context, int centerX, int centerY, int radius)
{
    for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            if(x * x + y * y > radius * radius)
            {
                continue;
			}

			int worldX = centerX + x;
            int worldY = centerY + y;

            if (worldX <= 0 || worldX >= context.width - 1 || worldY <= 0 || worldY >= context.height - 1)
            {
                continue;
            }

			context.map.GetBlockUnsafe(worldX, worldY).type = Block::Type::Air;
        }
    }
}

int GenerateDesertDuneHeight(WorldGenContext& context, int x)
{
    const int baseHeight{ context.surfaceHeights.at(std::max(0, x - 1)) };

    const float amplitude{ 1.02f }; //Dune height
    const float frequency{ 0.035f }; //Dune width
    const float phase = context.seed * 0.01f; //Dune horizontal shift based on seed

    float dune = sinf(x * frequency + phase) * amplitude;

    return baseHeight + static_cast<int>(dune);
}

BiomeType GetBiomeTypeAtX(WorldGenContext& context, int x)
{

    for (const BiomeRegion& region : context.biomeRegions)
    {
        if (x >= region.startX && x <= region.endX)
        {
            return region.type;
        }
    }

    return BiomeType::Forest;
}

BiomeType GetBiomeTypeForBlock(WorldGenContext& context, int x)
{
    const int transitionWidth = 30;

    for (int i = 0; i < context.biomeRegions.size(); i++)
    {
        const BiomeRegion& region = context.biomeRegions[i];

        if (x >= region.startX && x <= region.endX)
        {
            // Near left edge, blend with previous biome
            if (i > 0 && x < region.startX + transitionWidth)
            {
                const BiomeRegion& previousRegion = context.biomeRegions[i - 1];

                float t = static_cast<float>(x - region.startX) / transitionWidth;

                // At start: mostly previous biome.
                // At end: mostly current biome.
                if (GetRandomChance(context.rng, t))
                {
                    return region.type;
                }
                else
                {
                    return previousRegion.type;
                }
            }

            return region.type;
        }
    }

    return BiomeType::Forest;
}

void GenerateBiomeRegions(WorldGenContext& context)
{
	context.biomeRegions.clear();

    context.biomeRegions.emplace_back(BiomeType::Forest, 0, 299);
    context.biomeRegions.emplace_back(BiomeType::Desert, 300, 499);
    context.biomeRegions.emplace_back(BiomeType::Snow, 500, context.width - 1);

}

void GenerateTerrainHeights(WorldGenContext& context)
{
	int surfaceHeight{ 150 };
	int stoneDepth{ 50 };

	int surfaceDirection{ 0 };
	int surfaceDirectionTime{ 0 };

	int stoneDirection{ 0 };
	int stoneDirectionTime{ 0 };

    for (int x = 0; x < context.width; x++)
    {
		BiomeType biomeType = GetBiomeTypeAtX(context, x);
        Biome biome = GetBiome(biomeType);


        if (biomeType == BiomeType::Desert)
        {
            surfaceHeight = GenerateDesertDuneHeight(context, x);
        }
        else
        {
            //** Surface walker **//

            surfaceDirectionTime--;

            if (surfaceDirectionTime <= 0)
            {
                surfaceDirection = GetRandomInt(context.rng, -biome.maxStepAmount, biome.maxStepAmount);
                surfaceDirectionTime = GetRandomInt(context.rng, biome.minDirectionTime, biome.maxDirectionTime);
            }

            if (surfaceDirection < 0)
            {
                if (GetRandomChance(context.rng, biome.heightChangeChance))
                {
                    surfaceHeight--;
                }
            }
            else if (surfaceDirection > 0)
            {
                if (GetRandomChance(context.rng, biome.heightChangeChance))
                {
                    surfaceHeight++;
                }
            }

            if (surfaceHeight < biome.minSurfaceHeight)
            {
                surfaceHeight = biome.minSurfaceHeight;
            }

            if (surfaceHeight > biome.maxSurfaceHeight)
            {
                surfaceHeight = biome.maxSurfaceHeight;
            }
        }

        //** Stone walker **//

        stoneDirectionTime--;

        if (stoneDirectionTime <= 0)
        {
            stoneDirection = GetRandomInt(context.rng, -biome.stoneMaxStepAmount, biome.stoneMaxStepAmount);
            stoneDirectionTime = GetRandomInt(
                context.rng,
                biome.stoneMinDirectionTime,
                biome.stoneMaxDirectionTime
            );
        }

        if (stoneDirection < 0)
        {
            if (GetRandomChance(context.rng, biome.stoneDepthChangeChance))
            {
                stoneDepth--;
            }
        }
        else if (stoneDirection > 0)
        {
            if (GetRandomChance(context.rng, biome.stoneDepthChangeChance))
            {
                stoneDepth++;
            }
        }

        if (stoneDepth < biome.minStoneDepth)
        {
            stoneDepth = biome.minStoneDepth;
        }

        if (stoneDepth > biome.maxStoneDepth)
        {
            stoneDepth = biome.maxStoneDepth;
        }

        context.surfaceHeights.at(x) = surfaceHeight;
        context.stoneHeights.at(x) = surfaceHeight + stoneDepth;
    }
}

void SmoothTerrainHeights(WorldGenContext& context, int passes)
{
    for (int pass = 0; pass < passes; pass++)
    {
        std::vector<int> newSurface = context.surfaceHeights;
        std::vector<int> newStone = context.stoneHeights;

        for (int x = 1; x < context.width - 1; x++)
        {
            newSurface[x] =
                (context.surfaceHeights[x - 1] +
                    context.surfaceHeights[x] +
                    context.surfaceHeights[x + 1]) / 3;

            newStone[x] =
                (context.stoneHeights[x - 1] +
                    context.stoneHeights[x] +
                    context.stoneHeights[x + 1]) / 3;
        }

        context.surfaceHeights = newSurface;
        context.stoneHeights = newStone;
    }
}

void FillBaseTerrain(WorldGenContext& context)
{
    for (int x = 0; x < context.width; x++)
    {
        BiomeType biomeType = GetBiomeTypeAtX(context, x);
        Biome biome = GetBiome(biomeType);

        int surfaceHeight = context.surfaceHeights.at(x);
        int stoneHeight = context.stoneHeights.at(x);

        for (int y = 0; y < context.height; y++)
        {
            int fgvariation{ GetRandomInt(context.rng, 0, 3) };
            int bgvariation{ GetRandomInt(context.rng, 0, 3) };

            Block fgblock{};
            fgblock.type = Block::Type::Air;
            fgblock.variant = fgvariation;

            Block bgblock{};
            bgblock.type = Block::Type::Air;
            bgblock.variant = bgvariation;

            if (y < surfaceHeight)
            {
                fgblock.type = Block::Type::Air;
                bgblock.type = Block::Type::Air;
            }
            else if (y == surfaceHeight)
            {
                fgblock.type = biome.surfaceBlock;
                bgblock.type = Block::Type::Air;
            }
            else if (y < stoneHeight)
            {
                fgblock.type = biome.dirtBlock;
                bgblock.type = biome.wallBlock;
            }
            else
            {
                fgblock.type = Block::Type::Stone;
                bgblock.type = Block::Type::StoneWall;
            }

            context.map.GetBlockUnsafe(x, y) = fgblock;
            context.map.GetBackgroundBlockUnsafe(x, y) = bgblock;
        }
    }
}

void CarveCaves(WorldGenContext& context, float* caveNoise, float* cavePatchNoise)
{
    for (int x = 0; x < context.width; x++)
    {
        for (int y = 0; y < context.height; y++)
        {
            float caveValue = caveNoise[y * context.width + x];
            float cavePatchValue = cavePatchNoise[y * context.width + x];

            if (cavePatchValue > 0.3f && caveValue > 0.6f && caveValue < 0.8f)
            {
                context.map.GetBlockUnsafe(x, y).type = Block::Type::Air;
            }
        }
    }
}

void AddPerlinWorms(WorldGenContext& context, float* perlinNoise)
{
	int nrOfWorms = GetRandomInt(context.rng, 10, 30);

    std::vector<PerlinWorm> worms;
	worms.resize(nrOfWorms);

    for (PerlinWorm& worm : worms)
    {
        worm.x = GetRandomInt(context.rng, 10, context.width - 10);
        worm.y = GetRandomInt(context.rng, 51, context.height - 10);

        worm.length = GetRandomInt(context.rng, 200, 700);
		worm.radius = GetRandomInt(context.rng, 2, 5);

        Vector2 direction{
            GetRandomFloat(context.rng, -1.0f, 1.0f),
            GetRandomFloat(context.rng, -1.0f, 1.0f)
        };

		direction = Vector2Normalize(direction);

        worm.directionX = direction.x;
        worm.directionY = direction.y;
	}

    for (PerlinWorm& worm : worms)
    {
        while (worm.length > 0)
        {
            int x = static_cast<int>(worm.x);
            int y = static_cast<int>(worm.y);

			CarveCircle(context, x, y, worm.radius);

			float turnAmount = GetRandomFloat(context.rng, -0.18f, 0.18f);

			float oldDirectionX = worm.directionX;
			float oldDirectionY = worm.directionY;

			worm.directionX = oldDirectionX * cosf(turnAmount) - oldDirectionY * sinf(turnAmount);
			worm.directionY = oldDirectionX * sinf(turnAmount) + oldDirectionY * cosf(turnAmount);

			worm.x += worm.directionX;
			worm.y += worm.directionY;

			worm.length--;
        }
    }
}



void GenerateWorld(GameMap& map, int seed)
{
    const int width{ 900 };
    const int height{ 500 };

    map.Create(width, height);

	WorldGenContext context
    {
        map, 
        width, 
        height, 
        seed, 
        std::ranlux24_base{ static_cast<std::ranlux24_base::result_type>(seed) }
    };

	context.surfaceHeights.resize(width);
	context.stoneHeights.resize(width);

	GenerateBiomeRegions(context);

    std::unique_ptr<FastNoiseSIMD> plainsNoiseGenerator{ FastNoiseSIMD::NewFastNoiseSIMD() };
    std::unique_ptr<FastNoiseSIMD> mountainNoiseGenerator{ FastNoiseSIMD::NewFastNoiseSIMD() };
    std::unique_ptr<FastNoiseSIMD> biomeNoiseGenerator{ FastNoiseSIMD::NewFastNoiseSIMD() };
    std::unique_ptr<FastNoiseSIMD> stoneNoiseGenerator{ FastNoiseSIMD::NewFastNoiseSIMD() };
    std::unique_ptr<FastNoiseSIMD> caveNoiseGenerator{ FastNoiseSIMD::NewFastNoiseSIMD() };
    std::unique_ptr<FastNoiseSIMD> cavePatchNoiseGenerator{ FastNoiseSIMD::NewFastNoiseSIMD() };
	std::unique_ptr<FastNoiseSIMD> perlinWormNoiseGenerator{ FastNoiseSIMD::NewFastNoiseSIMD() };

    plainsNoiseGenerator->SetSeed(seed++);
    mountainNoiseGenerator->SetSeed(seed++);
    biomeNoiseGenerator->SetSeed(seed++);
    stoneNoiseGenerator->SetSeed(seed++);
	caveNoiseGenerator->SetSeed(seed++);
	cavePatchNoiseGenerator->SetSeed(seed++);
	perlinWormNoiseGenerator->SetSeed(seed++);

    plainsNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
    plainsNoiseGenerator->SetFractalOctaves(2);
    plainsNoiseGenerator->SetFrequency(0.01f);

    mountainNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
    mountainNoiseGenerator->SetFractalOctaves(5);
    mountainNoiseGenerator->SetFrequency(0.02f);

    biomeNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
    biomeNoiseGenerator->SetFractalOctaves(1);
    biomeNoiseGenerator->SetFrequency(0.001f); // very low = large biome areas

    stoneNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
    stoneNoiseGenerator->SetFractalOctaves(4);
    stoneNoiseGenerator->SetFrequency(0.03f);

    caveNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
    caveNoiseGenerator->SetFractalOctaves(3);
    caveNoiseGenerator->SetFrequency(0.02f);

    cavePatchNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
    cavePatchNoiseGenerator->SetFractalOctaves(2);
    cavePatchNoiseGenerator->SetFrequency(0.01f);

	perlinWormNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::PerlinFractal);
	perlinWormNoiseGenerator->SetFractalOctaves(3);
	perlinWormNoiseGenerator->SetFrequency(0.05f);

    float* plainsNoise = FastNoiseSIMD::GetEmptySet(width);
    float* mountainNoise = FastNoiseSIMD::GetEmptySet(width);
    float* biomeNoise = FastNoiseSIMD::GetEmptySet(width);
    float* stoneNoise = FastNoiseSIMD::GetEmptySet(width);
	float* caveNoise = FastNoiseSIMD::GetEmptySet(width * height);
	float* cavePatchNoise = FastNoiseSIMD::GetEmptySet(width * height);
	float* perlinWormNoise = FastNoiseSIMD::GetEmptySet(width * height);

    plainsNoiseGenerator->FillNoiseSet(plainsNoise, 0, 0, 0, width, 1, 1);
    mountainNoiseGenerator->FillNoiseSet(mountainNoise, 0, 0, 0, width, 1, 1);
    biomeNoiseGenerator->FillNoiseSet(biomeNoise, 0, 0, 0, width, 1, 1);
    stoneNoiseGenerator->FillNoiseSet(stoneNoise, 0, 0, 0, width, 1, 1);
	caveNoiseGenerator->FillNoiseSet(caveNoise, 0, 0, 0, height, width, 1);
	cavePatchNoiseGenerator->FillNoiseSet(cavePatchNoise, 0, 0, 0, height, width, 1);
	perlinWormNoiseGenerator->FillNoiseSet(perlinWormNoise, 0, 0, 0, height, width, 1);

    for (int i = 0; i < width; i++)
    {
        plainsNoise[i] = (plainsNoise[i] + 1.0f) / 2.0f;
        mountainNoise[i] = (mountainNoise[i] + 1.0f) / 2.0f;
        biomeNoise[i] = (biomeNoise[i] + 1.0f) / 2.0f;
        stoneNoise[i] = (stoneNoise[i] + 1.0f) / 2.0f;
    }

    for (int i = 0; i < width * height; i++)
    {
		caveNoise[i] = (caveNoise[i] + 1.0f) / 2.0f;
		cavePatchNoise[i] = (cavePatchNoise[i] + 1.0f) / 2.0f;
    }

	GenerateTerrainHeights(context);

	//moothTerrainHeights(context, 5);

	FillBaseTerrain(context);

	CarveCaves(context, caveNoise, cavePatchNoise);

	AddPerlinWorms(context, perlinWormNoise);

    FastNoiseSIMD::FreeNoiseSet(plainsNoise);
    FastNoiseSIMD::FreeNoiseSet(mountainNoise);
    FastNoiseSIMD::FreeNoiseSet(biomeNoise);
    FastNoiseSIMD::FreeNoiseSet(stoneNoise);
	FastNoiseSIMD::FreeNoiseSet(caveNoise);
	FastNoiseSIMD::FreeNoiseSet(cavePatchNoise);
	FastNoiseSIMD::FreeNoiseSet(perlinWormNoise);
}