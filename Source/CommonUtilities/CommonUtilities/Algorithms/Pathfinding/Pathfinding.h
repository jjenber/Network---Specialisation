#pragma once
#include <limits.h>

namespace CommonUtilities
{
	static const int MapWidth = 20;
	static const int MapHeight = 20;
	static const int TileCount = MapWidth * MapHeight;

	enum class Direction
	{
		West,
		East,
		North,
		South,
	};

	enum class Tile
	{
		Impassable,
		Passable
	};
	class PathFinding
	{
	public:
		static int GetNeighbor(Direction aDirection, int aIndexOrigin);
	};
}