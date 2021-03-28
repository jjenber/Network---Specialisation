#pragma once
#include <Math/Vector3.h>

namespace CommonUtilities
{
	template <typename T>
	class AABB3D
	{
	public:
		AABB3D() = default;
		AABB3D(const AABB3D<T>& aAABB3D);
		AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax);
		
		void InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax);
		bool IsInside(const Vector3<T>& aPosition) const;
	
		Vector3<T> myMin;
		Vector3<T> myMax;
	};

	template<typename T>
	inline AABB3D<T>::AABB3D(const AABB3D<T>& aAABB3D)
	{
		myMin = aAABB3D.myMin;
		myMax = aAABB3D.myMax;
	}

	template<typename T>
	inline AABB3D<T>::AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		myMin = aMin;
		myMax = aMax;
	}

	template<typename T>
	inline void AABB3D<T>::InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		myMin = aMin;
		myMax = aMax;
	}
	template<typename T>
	inline bool AABB3D<T>::IsInside(const Vector3<T>& aPosition) const
	{
		return 
			aPosition.x >= myMin.x && aPosition.x <= myMax.x &&
			aPosition.y >= myMin.y && aPosition.y <= myMax.y &&
			aPosition.z >= myMin.z && aPosition.z <= myMax.z;
	}
}
