#include "worldGenerator.h"
#include "randomStuff.h"
#include <raymath.h>
#include <FastNoiseSIMD.h>
#include "biomes.h"
#include <vector>

#include "worldGenContext.h"

BiomeType GetBiomeTypeAtX(const std::vector<BiomeRegion>& regions, int x)
{
    for (const BiomeRegion& region : regions)
    {
        if (x >= region.startX && x <= region.endX)
        {
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

void GenerateTerrainHeights(WorldGenContext& context, float* plainsNoise, float* mountainNoise, float* biomeNoise, float* stoneNoise)
{
    const int stoneDepthMin = 20;
    const int stoneDepthMax = 60;

    for (int x = 0; x < context.width; x++)
    {
        BiomeType biomeType = GetBiomeTypeAtX(context.biomeRegions, x);
        Biome biome = GetBiome(biomeType);

        int plainsHeight = 140 + static_cast<int>(plainsNoise[x] * 8);
        int mountainHeight = 80 + static_cast<int>(
            mountainNoise[x] * 90 * biome.terrainHeightMultiplier
            );

        float biomeValue = biomeNoise[x] * 0.55f;

        int surfaceHeight = static_cast<int>(
            Lerp(
                static_cast<float>(plainsHeight),
                static_cast<float>(mountainHeight),
                biomeValue
            )
            );

        int stoneDepth = stoneDepthMin + static_cast<int>((stoneDepthMax - stoneDepthMin) * stoneNoise[x]);
        int stoneHeight = surfaceHeight + stoneDepth;

        context.surfaceHeights.at(x) = surfaceHeight;
        context.stoneHeights.at(x) = stoneHeight;

    }
}

void FillBaseTerrain(WorldGenContext& context)
{
    for (int x = 0; x < context.width; x++)
    {
        BiomeType biomeType = GetBiomeTypeAtX(context.biomeRegions, x);
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

    plainsNoiseGenerator->SetSeed(seed++);
    mountainNoiseGenerator->SetSeed(seed++);
    biomeNoiseGenerator->SetSeed(seed++);
    stoneNoiseGenerator->SetSeed(seed++);
	caveNoiseGenerator->SetSeed(seed++);
	cavePatchNoiseGenerator->SetSeed(seed++);

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

    float* plainsNoise = FastNoiseSIMD::GetEmptySet(width);
    float* mountainNoise = FastNoiseSIMD::GetEmptySet(width);
    float* biomeNoise = FastNoiseSIMD::GetEmptySet(width);
    float* stoneNoise = FastNoiseSIMD::GetEmptySet(width);
	float* caveNoise = FastNoiseSIMD::GetEmptySet(width * height);
	float* cavePatchNoise = FastNoiseSIMD::GetEmptySet(width * height);

    plainsNoiseGenerator->FillNoiseSet(plainsNoise, 0, 0, 0, width, 1, 1);
    mountainNoiseGenerator->FillNoiseSet(mountainNoise, 0, 0, 0, width, 1, 1);
    biomeNoiseGenerator->FillNoiseSet(biomeNoise, 0, 0, 0, width, 1, 1);
    stoneNoiseGenerator->FillNoiseSet(stoneNoise, 0, 0, 0, width, 1, 1);
	caveNoiseGenerator->FillNoiseSet(caveNoise, 0, 0, 0, height, width, 1);
	cavePatchNoiseGenerator->FillNoiseSet(cavePatchNoise, 0, 0, 0, height, width, 1);

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

	GenerateTerrainHeights(context, plainsNoise, mountainNoise, biomeNoise, stoneNoise);

	FillBaseTerrain(context);

	CarveCaves(context, caveNoise, cavePatchNoise);

    FastNoiseSIMD::FreeNoiseSet(plainsNoise);
    FastNoiseSIMD::FreeNoiseSet(mountainNoise);
    FastNoiseSIMD::FreeNoiseSet(biomeNoise);
    FastNoiseSIMD::FreeNoiseSet(stoneNoise);
	FastNoiseSIMD::FreeNoiseSet(caveNoise);
	FastNoiseSIMD::FreeNoiseSet(cavePatchNoise);
}