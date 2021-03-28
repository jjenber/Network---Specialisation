#pragma once
#include "Plane.h"
#include <vector>
namespace CommonUtilities
{
	template<typename T>
	class PlaneVolume
	{
	public:
		PlaneVolume() = default;
		PlaneVolume(const std::vector<Plane<T>>& aLineList);
		void AddPlane(const Plane<T>& aLine);
		bool IsInside(const Vector3<T>& aPosition);

	private:
		std::vector<Plane<T>> myPlanes;
	};

	template<typename T>
	inline PlaneVolume<T>::PlaneVolume(const std::vector<Plane<T>>& aPlaneList)
	{
		myPlanes = aPlaneList;
	}

	template<typename T>
	inline void PlaneVolume<T>::AddPlane(const Plane<T>& aPlane)
	{
		myPlanes.push_back(aPlane);
	}
	template<typename T>
	inline bool PlaneVolume<T>::IsInside(const Vector3<T>& aPosition)
	{
		for (int i = 0; i < myPlanes.size(); i++)
		{
			if (!myPlanes[i].IsInside(aPosition))
			{
				return false;
			}
		}
		return true;
	}
}
