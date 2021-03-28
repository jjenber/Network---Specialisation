#include "Dijkstra.hpp"
#include <Containers/MinHeap.hpp>

namespace CommonUtilities
{
	std::vector<int> Dijkstra(const std::vector<Tile>& aMap, int aStartIndex, int anEndIndex)
	{
		std::vector<TraversalNode> path;
		path.resize(aMap.size());

		MinHeap<TraversalNode> toBeChecked(static_cast<int>(aMap.size()));
		toBeChecked.Enqueue(TraversalNode(-1, aStartIndex, 0));

		bool hasBeenChecked[TileCount];
		std::memset(hasBeenChecked, 0, sizeof(bool) * TileCount);

		while (!toBeChecked.IsEmpty())
		{
			TraversalNode v = toBeChecked.Dequeue();
			if (hasBeenChecked[v.myTileIndex])
				continue;
			hasBeenChecked[v.myTileIndex] = true;


			int west = GetNeighbor(Direction::West, v.myTileIndex);
			if (west != -1 && !hasBeenChecked[west] && aMap[west] == Tile::Passable)
			{
				toBeChecked.Enqueue(TraversalNode(v.myTileIndex, west, v.myDistance + 1));
			}

			int east = GetNeighbor(Direction::East, v.myTileIndex);
			if (east != -1 && !hasBeenChecked[east] && aMap[east] == Tile::Passable)
			{
				toBeChecked.Enqueue(TraversalNode(v.myTileIndex, east, v.myDistance + 1));
			}

			int south = GetNeighbor(Direction::South, v.myTileIndex);
			if (south != -1 && !hasBeenChecked[south] && aMap[south] == Tile::Passable)
			{
				toBeChecked.Enqueue(TraversalNode(v.myTileIndex, south, v.myDistance + 1));
			}

			int north = GetNeighbor(Direction::North, v.myTileIndex);
			if (north != -1 && !hasBeenChecked[north] && aMap[north] == Tile::Passable)
			{
				toBeChecked.Enqueue(TraversalNode(v.myTileIndex, north, v.myDistance + 1));
			}

			path[v.myTileIndex] = v;
			if (v.myTileIndex == anEndIndex)
			{
				break;
			}
		}

		std::vector<int> result;
		if (path[anEndIndex].myPredecessor != -1)
		{
			int nextTileIndex = anEndIndex;
			while (nextTileIndex != -1)
			{
				result.push_back(path[nextTileIndex].myTileIndex);
				nextTileIndex = path[nextTileIndex].myPredecessor;
			}
		}
		
		return std::vector<int>(result.rbegin(), result.rend());
	}

	int GetNeighbor(Direction aDirection, int aIndexOrigin)
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

	TraversalNode::TraversalNode(int aPredecessor, const int aIndex, const int aDistance)
		:	myPredecessor(aPredecessor), myTileIndex(aIndex), myDistance(aDistance)
	{}

	bool TraversalNode::operator<(const TraversalNode& aOther)
	{
		return myDistance < aOther.myDistance;
	}

}