#include "worldGenerator.h"
#include "randomStuff.h"
#include <raymath.h>
#include <FastNoiseSIMD.h>

float lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

void GenerateWorld(GameMap& map, int seed)
{
    const int width{ 900 };
    const int height{ 500 };

    map.Create(width, height);

    std::unique_ptr<FastNoiseSIMD> plainsNoiseGenerator{ FastNoiseSIMD::NewFastNoiseSIMD() };
    std::unique_ptr<FastNoiseSIMD> mountainNoiseGenerator{ FastNoiseSIMD::NewFastNoiseSIMD() };
    std::unique_ptr<FastNoiseSIMD> biomeNoiseGenerator{ FastNoiseSIMD::NewFastNoiseSIMD() };
    std::unique_ptr<FastNoiseSIMD> stoneNoiseGenerator{ FastNoiseSIMD::NewFastNoiseSIMD() };

    plainsNoiseGenerator->SetSeed(seed++);
    mountainNoiseGenerator->SetSeed(seed++);
    biomeNoiseGenerator->SetSeed(seed++);
    stoneNoiseGenerator->SetSeed(seed++);

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

    float* plainsNoise = FastNoiseSIMD::GetEmptySet(width);
    float* mountainNoise = FastNoiseSIMD::GetEmptySet(width);
    float* biomeNoise = FastNoiseSIMD::GetEmptySet(width);
    float* stoneNoise = FastNoiseSIMD::GetEmptySet(width);

    plainsNoiseGenerator->FillNoiseSet(plainsNoise, 0, 0, 0, width, 1, 1);
    mountainNoiseGenerator->FillNoiseSet(mountainNoise, 0, 0, 0, width, 1, 1);
    biomeNoiseGenerator->FillNoiseSet(biomeNoise, 0, 0, 0, width, 1, 1);
    stoneNoiseGenerator->FillNoiseSet(stoneNoise, 0, 0, 0, width, 1, 1);

    for (int i = 0; i < width; i++)
    {
        plainsNoise[i] = (plainsNoise[i] + 1.0f) / 2.0f;
        mountainNoise[i] = (mountainNoise[i] + 1.0f) / 2.0f;
        biomeNoise[i] = (biomeNoise[i] + 1.0f) / 2.0f;
        stoneNoise[i] = (stoneNoise[i] + 1.0f) / 2.0f;
    }

    const int surfaceMin = 80;
    const int surfaceMax = 180;

    const int stoneDepthMin = 20;
    const int stoneDepthMax = 60;

    for (int x = 0; x < width; x++)
    {
        int plainsHeight = 140 + static_cast<int>(plainsNoise[x] * 8);
        int mountainHeight = 80 + static_cast<int>(mountainNoise[x] * 90);

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

        for (int y = 0; y < height; y++)
        {
            Block block{};
            block.type = Block::Type::Air;

            if (y < surfaceHeight)
            {
                block.type = Block::Type::Air;
            }
            else if (y == surfaceHeight)
            {
                block.type = Block::Type::GrassBlock;
            }
            else if (y < stoneHeight)
            {
                block.type = Block::Type::Dirt;
            }
            else
            {
                block.type = Block::Type::Stone;
            }

            map.GetBlockUnsafe(x, y) = block;
        }
    }

    FastNoiseSIMD::FreeNoiseSet(plainsNoise);
    FastNoiseSIMD::FreeNoiseSet(mountainNoise);
    FastNoiseSIMD::FreeNoiseSet(biomeNoise);
    FastNoiseSIMD::FreeNoiseSet(stoneNoise);
}