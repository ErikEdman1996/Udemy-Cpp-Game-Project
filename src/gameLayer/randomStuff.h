#pragma once
#include <random>

// Returns a random float between min and max using the provided random number generator
float GetRandomFloat(std::ranlux24_base& rng, float min, float max);

// Returns a random integer between min and max (inclusive) using the provided random number generator
int GetRandomInt(std::ranlux24_base& rng, int min, int max);

// Returns true with the given chance (between 0.0 and 1.0) using the provided random number generator
bool GetRandomChance(std::ranlux24_base& rng, float chance);