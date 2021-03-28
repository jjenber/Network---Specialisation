#include "Random.h"
#include "PerlinNoise.hpp"

static std::default_random_engine generator;
static unsigned int ourSeed = generator.default_seed;
static BasicPerlinNoise<float> ourPerlinNoise = BasicPerlinNoise<float>(ourSeed);

unsigned int Random::GetSeed()
{
	return ourSeed;
}

void Random::SetSeed(unsigned int aSeed)
{
	ourSeed = aSeed;
	generator.seed(aSeed);
	ourPerlinNoise.reseed(aSeed);
}

// Returns a random int. aMin [inclusive], aMax [inclusive].
float Random::Range(const float aMin, const float aMax)
{
	float min = aMin;
	float max = aMax;
	if (max < min)
	{
		min = aMax;
		max = aMin;
	}
	std::uniform_real_distribution<float> dist(min, max);
	return dist(generator);
}

// Returns a random int. aMin [inclusive], aMax [exclusive].
int Random::Range(const int aMin, const int aMax)
{
	int min = aMin;
	int max = aMax;
	if (max < min)
	{
		min = aMax;
		max = aMin;
	}
	std::uniform_int_distribution<int> dist(min, max - 1);
	return dist(generator);
}

float Random::PerlinNoise2D(const float aX, const float aY)
{
	return ourPerlinNoise.noise3D(aX, aY, 0);
}
float Random::PerlinNoise2D_0_1(const float aX, const float aY)
{
	return ourPerlinNoise.noise2D_0_1(aX, aY);
}
float Random::PerlinNoise3D(const float aX, const float aY, const float aZ)
{
	return ourPerlinNoise.noise3D(aX, aY, aZ);
}
float Random::PerlinNoise3D_0_1(const float aX, const float aY, const float aZ)
{
	return ourPerlinNoise.noise3D_0_1(aX, aY, aZ);
}

