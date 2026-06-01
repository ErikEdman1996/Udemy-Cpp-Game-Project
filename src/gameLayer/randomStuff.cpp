#include "randomStuff.h"

float GetRandomFloat(std::ranlux24_base& rng, float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}

int GetRandomInt(std::ranlux24_base& rng, int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(rng);
}

bool GetRandomChance(std::ranlux24_base& rng, float chance)
{
	float dice = GetRandomFloat(rng, 0.0f, 1.0f);
	return dice <= chance;
}