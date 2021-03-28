#pragma once
#include <Math/Vector2.h>

namespace CommonUtilities
{
	template <typename T>
	class Line
	{
	public:
		Line() = default;
		Line(const Line<T>& aLine);
		Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
		
		void						InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
		void						InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T> aDirection);

		bool						IsInside(const Vector2<T>& aPosition) const;
		
		inline const Vector2<T>&	GetNormal() const;
		inline const Vector2<T>		GetDirection() const;
	private:
		Vector2<T>					myNormal;
		Vector2<T>					myPoint;
	};

	template<typename T>
	inline Line<T>::Line(const Line<T>& aLine)
	{
		myNormal = aLine.myNormal;
		myPoint = aLine.myPoint;
	}

	template<typename T>
	inline Line<T>::Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		InitWith2Points(aPoint0, aPoint1);
	}

	template<typename T>
	inline void Line<T>::InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		myNormal = Vector2<T>{ -(aPoint1.y - aPoint0.y), (aPoint1.x - aPoint0.x) }.GetNormalized();
		myPoint = aPoint0;
	}

	template<typename T>
	inline void Line<T>::InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T> aDirection)
	{
		myNormal = Vector2<T>{ -aDirection.y, aDirection.x }.GetNormalized();
		myPoint = aPoint;
	}

	template<typename T>
	bool Line<T>::IsInside(const Vector2<T>& aPosition) const
	{
		return myNormal.Dot(aPosition - myPoint) <= 0;
	}

	template<typename T>
	inline const Vector2<T>& Line<T>::GetNormal() const
	{
		return myNormal;
	}

	template<typename T>
	inline const Vector2<T> Line<T>::GetDirection() const
	{
		return { myNormal.y, -myNormal.x };
	}
}

