#pragma once

#include <random>
class Random
{
public:
	static unsigned int GetSeed();
	static void SetSeed(unsigned int aSeed);
	static float Range(const float aMin, const float aMax);
	static int   Range(const int aMin, const int aMax);

	// Returns a random value between -1, 1.
	static float PerlinNoise2D(const float aX, const float aY);
	// Returns a random value between 0, 1.
	static float PerlinNoise2D_0_1(const float aX, const float aY);
	// Returns a random value between -1, 1.
	static float PerlinNoise3D(const float aX, const float aY, const float aZ);
	// Returns a random value between 0, 1.
	static float PerlinNoise3D_0_1(const float aX, const float aY, const float aZ);
};
