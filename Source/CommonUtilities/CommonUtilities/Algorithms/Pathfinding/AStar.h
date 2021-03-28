#pragma once
#include <vector>
#include "Pathfinding.h"
namespace CommonUtilities
{
	enum class Heuristic
	{
		Manhattan,
		Euclidian,
	};

	struct NodeRecord
	{
		NodeRecord() = default;
		NodeRecord(const int aF, const int aG, const int aPredecessor, const int aIndex);
		inline bool operator<(const NodeRecord& aOther) { return myF < aOther.myF; };
		
		int myF = 0;
		int myPredecessor = -1;
		int myG = INT_MAX;
		int myIndex = -1;
	};

	std::vector<int> AStar(const std::vector<Tile>& aMap, int aStartIndex, int anEndIndex, Heuristic aHeuristic = Heuristic::Manhattan);
	std::vector<int> AStar_Debug(const std::vector<Tile>& aMap, int aStartIndex, int anEndIndex, Heuristic aHeuristic, std::vector<int>& aCheckedNodes);

	int GetNeighbor(Direction aDirection, int aIndexOrigin);
	int GetManhattanDistance(const int aEndNodeX, const int aEndNodeY, const int aCurrentNode);
	int GetEuclidianDistance(const int aEndNodeX, const int aEndNodeY, const int aCurrentNode);
}
