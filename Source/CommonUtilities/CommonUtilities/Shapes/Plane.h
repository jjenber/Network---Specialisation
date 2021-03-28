#pragma once
#include <Math/Vector3.h>
namespace CommonUtilities
{
	template <typename T>
	class Plane
	{
	public:
		Plane() = default;
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal);

		void InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);
		void InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal);
		
		bool IsInside(const Vector3<T>& aPosition) const;
		
		const Vector3<T>& GetNormal() const;
		const Vector3<T>& GetPoint() const;
		
	private:
		Vector3<T> myPoint;
		Vector3<T> myNormal;
	};

	template<typename T>
	inline Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
		: Plane(aPoint0, Vector3<T>{ aPoint1 - aPoint0 }.Cross(aPoint2 - aPoint0).GetNormalized())
	{
	}

	template<typename T>
	inline Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal)
	{
		myNormal = aNormal;
		myPoint = aPoint0;
	}

	template<typename T>
	inline void Plane<T>::InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
	{
		myNormal = Vector3<T>{ aPoint1 - aPoint0 }.Cross(aPoint2 - aPoint0).GetNormalized();
		myPoint = aPoint0;
	}

	template<typename T>
	inline void Plane<T>::InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal)
	{
		myNormal = aNormal;
		myPoint = aPoint;
	}

	template<typename T>
	inline const Vector3<T>& Plane<T>::GetNormal() const
	{
		return myNormal;
	}

	template<typename T>
	inline const Vector3<T>& Plane<T>::GetPoint() const
	{
		return myPoint;
	}

	template<typename T>
	inline bool Plane<T>::IsInside(const Vector3<T>& aPosition) const
	{
		auto val = myNormal.Dot(aPosition - myPoint) <= 0;
		return val;
	}
}