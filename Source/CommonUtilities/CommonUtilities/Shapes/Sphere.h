#pragma once
#include <Math/Vector3.h>

namespace CommonUtilities
{
	template <class T>
	class Sphere
	{
	public:
		Sphere() = default;
		Sphere(const Sphere<T>& aSphere);
		Sphere(const Vector3<T>& aCenter, T aRadius);

		void InitWithCenterAndRadius(const Vector3<T>& aCenter,const T aRadius);
		bool IsInside(const Vector3<T>& aPosition) const;
		const Vector3<T>& GetCenter() const;
		T GetRadius() const;

 	private:
		Vector3<T> myCenter;
		T myRadius;
	};
	template<class T>
	inline Sphere<T>::Sphere(const Sphere<T>& aSphere)
	{
		myCenter = aSphere.myCenter;
		myRadius = aSphere.myRadius;
	}
	template<class T>
	inline Sphere<T>::Sphere(const Vector3<T>& aCenter, T aRadius)
	{
		myCenter = aCenter;
		myRadius = aRadius;
	}
	template<class T>
	inline void Sphere<T>::InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius)
	{
		myCenter = aCenter;
		myRadius = aRadius;
	}
	template<class T>
	inline bool Sphere<T>::IsInside(const Vector3<T>& aPosition) const
	{
		return Vector3<T>::Distance(myCenter, aPosition) <= myRadius;
	}
	template<class T>
	__forceinline const Vector3<T>& Sphere<T>::GetCenter() const
	{
		return myCenter;
	}
	template<class T>
	__forceinline T Sphere<T>::GetRadius() const
	{
		return myRadius;
	}
}