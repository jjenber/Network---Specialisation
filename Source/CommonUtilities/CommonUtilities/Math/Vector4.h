#pragma once
#pragma once
#include <math.h>
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG

namespace CommonUtilities
{
	template <class T>
	class Vector4
	{
	public:
		union { T x; T myX; };
		union { T y; T myY; };
		union { T z; T myZ; };
		union { T w; T myW; };

		Vector4<T>();
		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW);
		Vector4<T>(const Vector4<T>& aVector4) = default;
		Vector4<T>& operator=(const Vector4<T>& aVector4) = default;

		~Vector4<T>() = default;

		T LengthSqr() const;
		T Length() const;

		Vector4<T> GetNormalized() const;
		void Normalize();

		T Dot(const Vector4<T>& aVector) const;
	};

	typedef Vector4<float> Vector4f;
	typedef Vector4<int> Vector4i;
	typedef Vector4<unsigned int>  Vector4ui;

	template <class T> Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1);
	template <class T> Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1);
	template <class T> Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar);
	template <class T> Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector);
	template <class T> Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar);
	template <class T> void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1);
	template <class T> void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1);
	template <class T> void operator*=(Vector4<T>& aVector, const T& aScalar);
	template <class T> void operator/=(Vector4<T>& aVector, const T& aScalar);


#pragma region MemberDefinitions
template<class T>
inline Vector4<T>::Vector4() : Vector4(0, 0, 0, 0)
{}

template<class T>
inline Vector4<T>::Vector4(const T& aX, const T& aY, const T& aZ, const T& aW) : x(aX), y(aY), z(aZ), w(aW)
{}

template<class T>
inline T Vector4<T>::LengthSqr() const
{
	return (x * x) + (y * y) + (z * z) + (w * w);
}

template<class T>
inline T Vector4<T>::Length() const
{
	return sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

template<class T>
inline Vector4<T> Vector4<T>::GetNormalized() const
{
	const T magnitude = (x * x) + (y * y) + (z * z) + (w * w);

#ifdef _DEBUG
	assert(magnitude != 0 && "Tried to normalize a null vector ");
#endif // _DEBUG

	const T inversedMagnitude = T(1) / sqrt(magnitude);
	return Vector4<T>(x * inversedMagnitude, y * inversedMagnitude, z * inversedMagnitude, w * inversedMagnitude);
}

template<class T>
inline void Vector4<T>::Normalize()
{
	const T magnitude = (x * x) + (y * y) + (z * z) + (w * w);

#ifdef _DEBUG
	assert(magnitude != 0 && "Tried to normalize a null vector ");
#endif // _DEBUG

	const T inversedMagnitude = T(1) / sqrt(magnitude);
	x = x * inversedMagnitude;
	y = y * inversedMagnitude;
	z = z * inversedMagnitude;
	w = w * inversedMagnitude;
}
template<class T>
inline T Vector4<T>::Dot(const Vector4<T>& aVector) const
{
	return (x * aVector.x) + (y * aVector.y) + (z * aVector.z) + (w * aVector.w);
}

#pragma endregion MemberDefinitions

#pragma region OperatorDefinitions

template <class T>
Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
{
	return Vector4<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z, aVector0.w + aVector1.w);
}

template<class T>
Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
{
	return Vector4<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z, aVector0.w - aVector1.w);
}

template <class T>
Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
{
	return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
}

template <class T>
Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
{
	return aVector * aScalar;
}

template <class T>
Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
{
	return aVector * (1 / aScalar);
}

template <class T>
void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
{
	aVector0.x += aVector1.x;
	aVector0.y += aVector1.y;
	aVector0.z += aVector1.z;
	aVector0.w += aVector1.w;
}

template <class T>
void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
{
	aVector0.x -= aVector1.x;
	aVector0.y -= aVector1.y;
	aVector0.z -= aVector1.z;
	aVector0.w -= aVector1.w;
}

template <class T>
void operator*=(Vector4<T>& aVector, const T& aScalar)
{
	aVector.x *= aScalar;
	aVector.y *= aScalar;
	aVector.z *= aScalar;
	aVector.w *= aScalar;
}

template <class T>
void operator/=(Vector4<T>& aVector, const T& aScalar)
{
	const T inv = (1 / aScalar);
	aVector.x *= inv;
	aVector.y *= inv;
	aVector.z *= inv;
	aVector.w *= inv;
}

#pragma endregion OperatorDefinitions


}
