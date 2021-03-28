#include "AStar.h"
#include <Containers/MinHeap.hpp>
#include <math.h>

namespace CommonUtilities
{
	std::vector<int> AStar(const std::vector<Tile>& aMap, int aStartIndex, int anEndIndex, Heuristic aHeuristic)
	{
		const int aEndNodeX = anEndIndex % MapWidth;
		const int aEndNodeY = anEndIndex / MapHeight;
		
		std::vector<NodeRecord> path(aMap.size());

		MinHeap<NodeRecord> openNodes(static_cast<int>(aMap.size()));
		openNodes.Enqueue(NodeRecord(0, 0, -1, aStartIndex));

		bool hasBeenChecked[TileCount];
		std::memset(hasBeenChecked, 0, sizeof(bool) * TileCount);

		auto heuristic =
			aHeuristic == Heuristic::Manhattan ? GetManhattanDistance :
			aHeuristic == Heuristic::Euclidian ? GetEuclidianDistance : nullptr;
		
		while (!openNodes.IsEmpty())
		{
			NodeRecord node = openNodes.Dequeue();
			if (hasBeenChecked[node.myIndex])
			{
				continue;
			}
			
			hasBeenChecked[node.myIndex] = true;
			if (node.myIndex == anEndIndex)
			{
				path[node.myIndex] = node;
				break;
			}

			const int g = node.myG + 1; // Calculate this with cost if needed. 
			int west = GetNeighbor(Direction::West, node.myIndex);
			if (west != -1 && !hasBeenChecked[west] && aMap[west] == Tile::Passable)
			{
				openNodes.Enqueue(NodeRecord(heuristic(aEndNodeX, aEndNodeY, west) + g, g, node.myIndex, west));
			}

			int east = GetNeighbor(Direction::East, node.myIndex);
			if (east != -1 && !hasBeenChecked[east] && aMap[east] == Tile::Passable)
			{
				openNodes.Enqueue(NodeRecord(heuristic(aEndNodeX, aEndNodeY, east) + g, g, node.myIndex, east));
			}

			int south = GetNeighbor(Direction::South, node.myIndex);
			if (south != -1 && !hasBeenChecked[south] && aMap[south] == Tile::Passable)
			{
				openNodes.Enqueue(NodeRecord(heuristic(aEndNodeX, aEndNodeY, south) + g, g, node.myIndex, south));
			}

			int north = GetNeighbor(Direction::North, node.myIndex);
			if (north != -1 && !hasBeenChecked[north] && aMap[north] == Tile::Passable)
			{
				openNodes.Enqueue(NodeRecord(heuristic(aEndNodeX, aEndNodeY, north) + g, g, node.myIndex, north));
			}

			path[node.myIndex] = node;
		}

		std::vector<int> result;
		if (path[anEndIndex].myPredecessor != -1)
		{
			int nextTileIndex = anEndIndex;
			while (nextTileIndex != -1)
			{
				result.push_back(path[nextTileIndex].myIndex);
				nextTileIndex = path[nextTileIndex].myPredecessor;
			}
		}

		return std::vector<int>(result.rbegin(), result.rend());
	}

	std::vector<int> AStar_Debug(const std::vector<Tile>& aMap, int aStartIndex, int anEndIndex, Heuristic aHeuristic, std::vector<int>& aCheckedNodes)
	{
		const int aEndNodeX = anEndIndex % MapWidth;
		const int aEndNodeY = anEndIndex / MapHeight;

		std::vector<NodeRecord> path(aMap.size());

		MinHeap<NodeRecord> openNodes(static_cast<int>(aMap.size()));
		openNodes.Enqueue(NodeRecord(0, 0, -1, aStartIndex));

		bool hasBeenChecked[TileCount];
		std::memset(hasBeenChecked, 0, sizeof(bool) * TileCount);

		auto heuristic =
			aHeuristic == Heuristic::Manhattan ? GetManhattanDistance :
			aHeuristic == Heuristic::Euclidian ? GetEuclidianDistance : nullptr;

		while (!openNodes.IsEmpty())
		{
			NodeRecord node = openNodes.Dequeue();
			if (hasBeenChecked[node.myIndex])
			{
				continue;
			}

			aCheckedNodes.push_back(node.myIndex);
			hasBeenChecked[node.myIndex] = true;
			if (node.myIndex == anEndIndex)
			{
				path[node.myIndex] = node;
				break;
			}

			const int g = node.myG + 1; // Calculate this with cost if needed. 
			int west = GetNeighbor(Direction::West, node.myIndex);
			if (west != -1 && !hasBeenChecked[west] && aMap[west] == Tile::Passable)
			{
				openNodes.Enqueue(NodeRecord(heuristic(aEndNodeX, aEndNodeY, west) + g, g, node.myIndex, west));
			}

			int east = GetNeighbor(Direction::East, node.myIndex);
			if (east != -1 && !hasBeenChecked[east] && aMap[east] == Tile::Passable)
			{
				openNodes.Enqueue(NodeRecord(heuristic(aEndNodeX, aEndNodeY, east) + g, g, node.myIndex, east));
			}

			int south = GetNeighbor(Direction::South, node.myIndex);
			if (south != -1 && !hasBeenChecked[south] && aMap[south] == Tile::Passable)
			{
				openNodes.Enqueue(NodeRecord(heuristic(aEndNodeX, aEndNodeY, south) + g, g, node.myIndex, south));
			}

			int north = GetNeighbor(Direction::North, node.myIndex);
			if (north != -1 && !hasBeenChecked[north] && aMap[north] == Tile::Passable)
			{
				openNodes.Enqueue(NodeRecord(heuristic(aEndNodeX, aEndNodeY, north) + g, g, node.myIndex, north));
			}

			path[node.myIndex] = node;
		}

		std::vector<int> result;
		if (path[anEndIndex].myPredecessor != -1)
		{
			int nextTileIndex = anEndIndex;
			while (nextTileIndex != -1)
			{
				result.push_back(path[nextTileIndex].myIndex);
				nextTileIndex = path[nextTileIndex].myPredecessor;
			}
		}

		return std::vector<int>(result.rbegin(), result.rend());
	}

	int GetManhattanDistance(const int aStartNodeX, const int aStartNodeY, const int aCurrentNode)
	{
		int currentNodeX = aCurrentNode % MapWidth;
		int currentNodeY = aCurrentNode / MapHeight;
		return std::abs(currentNodeX - aStartNodeX) + std::abs(currentNodeY - aStartNodeY);
	}

	int GetEuclidianDistance(const int aEndNodeX, const int aEndNodeY, const int aCurrentNode)
	{
		int currentNodeX = aCurrentNode % MapWidth;
		int currentNodeY = aCurrentNode / MapHeight;
		float xDistanceSqrd = static_cast<float>((currentNodeX - aEndNodeX)) * (currentNodeX - aEndNodeX);
		float yDistanceSqrd = static_cast<float>((currentNodeY - aEndNodeY)) * (currentNodeY - aEndNodeY);
		
		return static_cast<int>(roundf(sqrtf(xDistanceSqrd + yDistanceSqrd)));
	}

	NodeRecord::NodeRecord(const int aF, const int aG, const int aPredecessor, const int aIndex = 0)
		: myF(aF), myPredecessor(aPredecessor), myG(aG), myIndex(aIndex)
	{
	}
}
