#include "Pathfinding.h"

namespace CommonUtilities
{
	int PathFinding::GetNeighbor(Direction aDirection, int aIndexOrigin)
	{
		switch (aDirection)
		{
		case Direction::West:
		{
			int west = aIndexOrigin - 1;
			if (west == 0 || (west + 1) % MapWidth != 0)
			{
				return west;
			}
		} break;
		case Direction::East:
		{
			int east = aIndexOrigin + 1;
			if (east % (MapWidth) != 0)
			{
				return east;
			}
		} break;
		case Direction::North:
		{
			int north = aIndexOrigin - MapWidth;
			if (north >= 0)
			{
				return north;
			}
		} break;
		case Direction::South:
		{
			int south = aIndexOrigin + MapWidth;
			if (south < TileCount - 1)
				return south;
		}break;
		}
		return -1;
	}
}
