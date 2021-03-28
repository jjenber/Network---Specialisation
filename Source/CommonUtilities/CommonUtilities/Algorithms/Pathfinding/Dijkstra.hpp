#pragma once
#include <vector>

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
	struct TraversalNode
	{
		TraversalNode() = default;
		TraversalNode(int aPredecessor, const int aIndex, const int aDistance);
		bool operator<(const TraversalNode& aOther);
		int myPredecessor = -1;
		int myDistance = INT_MAX;
		int myTileIndex = -1;
	};

	std::vector<int> Dijkstra(const std::vector<Tile>& aMap, int aStartIndex, int anEndIndex);
	int GetNeighbor(Direction aDirection, int aIndexOrigin);
}
