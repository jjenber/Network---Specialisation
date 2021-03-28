#pragma once
#include <math.h>
#include <iostream>
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG

namespace CommonUtilities
{
	template <class T>
	class Vector2
	{
	public:
		union
		{
			T x;
			T myX;
		};
		union
		{
			T y;
			T myY;
		};

		Vector2<T>();
		~Vector2<T>() = default;
		Vector2<T>(const T& aX, const T& aY);
		Vector2<T>(const Vector2<T>& aVector) = default;
		
		template <class U>
		explicit operator Vector2<U>() { return { static_cast<U>(myX), static_cast<U>(myY) }; }

		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;
		
		template <class OtherType>
		Vector2<T>& operator=(const Vector2<OtherType>& aVector) { x = (T)aVector.x; y = (T)aVector.y; return *this; }

		T LengthSqr() const;
		T Length() const;

		Vector2<T> GetNormalized() const;
		Vector2<T>& Normalize();
		Vector2<T> Normal() const;

		T Dot(const Vector2<T>& aVector) const;
		
		void Set(const T& aX, const T& aY);

		template <class U>
		friend std::ostream& operator<<(std::ostream& os, const Vector2<U>& aVector);
	};

	typedef Vector2<float> Vector2f;
	typedef Vector2<int> Vector2i;
	typedef Vector2<unsigned int>  Vector2ui;

	template <class T> Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> Vector2<T> operator*(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar);
	template <class T> Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector);
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar);
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> bool operator==(const Vector2<T>& aVector0, const Vector2<T>& aVector1);

	template <class T> void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> void operator*=(Vector2<T>& aVector, const T& aScalar);
	template <class T> void operator/=(Vector2<T>& aVector, const T& aScalar);

#pragma region MemberDefinitions

	template<class T>
	inline Vector2<T>::Vector2() : Vector2(0, 0)
	{}

	template<class T>
	inline Vector2<T>::Vector2(const T& aX, const T& aY) : x(aX), y(aY)
	{}

	template<class T>
	inline T Vector2<T>::LengthSqr() const
	{
		return (x * x) + (y * y);
	}

	template<class T>
	inline T Vector2<T>::Length() const
	{
		return sqrt((x * x) + (y * y));
	}

	template<class T>
	inline Vector2<T> Vector2<T>::GetNormalized() const
	{
		const T magnitude = (x * x) + (y * y);

		if (magnitude == 0)
		{
			return *this;
		}

		const T inversedMagnitude = T(1) / sqrt(magnitude);
		return Vector2<T>(x * inversedMagnitude, y * inversedMagnitude);
	}

	template<class T>
	inline Vector2<T>& Vector2<T>::Normalize()
	{
		const T magnitude = (x * x) + (y * y);

		if (magnitude == 0)
		{
			return *this;
		}

		const T inversedMagnitude = T(1) / sqrt(magnitude);
		x *= inversedMagnitude; 
		y *= inversedMagnitude;
		return *this;
	}

	// Returns a copy of the non-normalized normal.
	template<class T> inline Vector2<T> Vector2<T>::Normal() const
	{
		return Vector2<T>(y, -x);
	}

	template<class T>
	inline T Vector2<T>::Dot(const Vector2<T>& aVector) const
	{
		return (x * aVector.x) + (y * aVector.y);
	}

	template<class T>
	inline void Vector2<T>::Set(const T& aX, const T& aY)
	{
		x = aX;
		y = aY;
	}

#pragma endregion MemberDefinitions

#pragma region OperatorDefinitions
	template <class T>
	Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y);
	}

	template<class T>
	Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y);
	}

	template<class T>
	Vector2<T> operator*(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.x * aVector1.x, aVector0.y * aVector1.y);
	}

	template <class T>
	Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
	}

	template <class T>
	Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
	{
		return aVector * aScalar;
	}

	template <class T>
	Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
	{
		return aVector * (1 / aScalar);
	}

	template<class T>
	Vector2<T> operator/(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>{ aVector0.x / aVector1.x, aVector0.y / aVector1.y };
	}

	template<class T>
	bool operator==(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return aVector0.x == aVector1.x && aVector0.y == aVector1.y;
	}

	template <class T>
	void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
	}

	template <class T>
	void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
	}

	template <class T>
	void operator*=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
	}

	template <class T>
	void operator/=(Vector2<T>& aVector, const T& aScalar)
	{
		const T inv = (1 / aScalar);
		aVector.x *= inv;
		aVector.y *= inv;
	}
	template <class T>
	std::ostream& operator<<(std::ostream& os, const Vector2<T>& aVector)
	{
		return os << "{ X: " << aVector.x << " Y: " << aVector.y << " }";
	}
#pragma endregion OperatorDefinitions
}