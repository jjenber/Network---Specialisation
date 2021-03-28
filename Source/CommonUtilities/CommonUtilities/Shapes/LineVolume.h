#pragma once
#include <vector>
#include "Line.h"
namespace CommonUtilities
{
	template<typename T>
	class LineVolume
	{
	public:
		LineVolume() = default;
		LineVolume(const std::vector<Line<T>>& aLineList);
		void AddLine(const Line<T>& aLine);
		bool IsInside(const Vector2<T>& aPosition);
	
	private:
		std::vector<Line<T>> myLines;
	};

	template<typename T>
	LineVolume<T>::LineVolume(const std::vector<Line<T>>& aLineList)
	{
		myLines = aLineList;
	}

	template<typename T>
	inline void LineVolume<T>::AddLine(const Line<T>& aLine)
	{
		myLines.push_back(aLine);
	}

	template<typename T>
	inline bool LineVolume<T>::IsInside(const Vector2<T>& aPosition)
	{
		for (int i = 0; i < myLines.size(); i++)
		{
			if (!myLines[i].IsInside(aPosition))
			{
				return false;
			}
		}
		return true;
	}
}