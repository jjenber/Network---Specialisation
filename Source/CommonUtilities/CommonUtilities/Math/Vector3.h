#pragma once
#include <math.h>
#include <assert.h>
namespace CommonUtilities
{
	template <class T>
	class Vector3
	{
	public:
		T x;
		T y;
		T z;

		Vector3<T>();
		Vector3<T>(const T& aX, const T& aY, const T& aZ);
		Vector3<T>(const Vector3<T>& aVector3) = default;
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;

		~Vector3<T>() = default;

		T LengthSqr() const;
		T Length() const;

		Vector3<T> GetNormalized() const;
		void Normalize();

		T Dot(const Vector3<T>& aVector) const;
		Vector3<T> Cross(const Vector3<T>& aVector) const;

		static Vector3<T> Abs(const Vector3<T>& aVector);
		static T Distance(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
		
		static Vector3<T> Up();
		static Vector3<T> Down();
		static Vector3<T> Left();
		static Vector3<T> Right();
		static Vector3<T> Forward();
		static Vector3<T> Back();
	};

	typedef Vector3<float> Vector3f;
	typedef Vector3<int> Vector3i;
	typedef Vector3<unsigned int>  Vector3ui;

	template <class T> Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
	template <class T> Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
	template <class T> Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar);
	template <class T> Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector);
	template <class T> Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar);
	template <class T> void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1);
	template <class T> void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1);
	template <class T> void operator*=(Vector3<T>& aVector, const T& aScalar);
	template <class T> void operator/=(Vector3<T>& aVector, const T& aScalar);

#pragma region MemberDefinitions

	template<class T>
	inline Vector3<T>::Vector3() : Vector3(0, 0, 0)
	{}

	template<class T>
	inline Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ) : x(aX), y(aY), z(aZ)
	{}

	template<class T>
	inline T Vector3<T>::LengthSqr() const
	{
		return (x * x) + (y * y) + (z * z);
	}

	template<class T>
	inline T Vector3<T>::Length() const
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}

	template<class T>
	inline Vector3<T> Vector3<T>::GetNormalized() const
	{
		const T magnitude = (x * x) + (y * y) + (z * z);

#ifdef _DEBUG
		assert(magnitude != 0 && "Tried to normalize a null vector ");
#endif // _DEBUG

		const T inversedMagnitude = T(1) / sqrt(magnitude);
		return Vector3<T>(x * inversedMagnitude, y * inversedMagnitude, z * inversedMagnitude);
	}

	template<class T>
	inline void Vector3<T>::Normalize()
	{
		const T magnitude = (x * x) + (y * y) + (z * z);

#ifdef _DEBUG
		assert(magnitude != 0 && "Tried to normalize a null vector ");
#endif // _DEBUG

		const T inversedMagnitude = T(1) / sqrt(magnitude);
		x = x * inversedMagnitude;
		y = y * inversedMagnitude;
		z = z * inversedMagnitude;
	}
	template<class T>
	inline T Vector3<T>::Dot(const Vector3<T>& aVector) const
	{
		return (x * aVector.x) + (y * aVector.y) + (z * aVector.z);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
	{
		return 
		{
			(y * aVector.z) - (z * aVector.y),
			(z * aVector.x) - (x * aVector.z),
			(x * aVector.y) - (y * aVector.x)
		};
	}
#pragma endregion MemberDefinitions

#pragma region OperatorDefinitions
	
	template <class T> 
	Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z);
	}

	template<class T> 
	Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z);
	}

	template <class T> 
	Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}

	template <class T>
	Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
	{
		return aVector * aScalar;
	}

	template <class T> 
	Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
	{
		return aVector * (1 / aScalar);
	}

	template <class T> 
	void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
	}

	template <class T> 
	void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
	}

	template <class T> 
	void operator*=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
	}

	template <class T> 
	void operator/=(Vector3<T>& aVector, const T& aScalar)
	{
		const T inv = (1 / aScalar);
		aVector.x *= inv;
		aVector.y *= inv;
		aVector.z *= inv;
	}

#pragma endregion OperatorDefinitions

#pragma region Static Functions
	template <class T>
	inline Vector3<T> Abs(const Vector3<T>& aVector)
	{
		return { std::abs(aVector.x), std::abs(aVector.y), std::abs(aVector.z) };
	}

	template <class T>
	inline T Vector3<T>::Distance(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		const Vector3<T> direction = aVector1 - aVector0;
		return std::sqrt(
		std::pow(direction.x, 2) + 
		std::pow(direction.y, 2) +
		std::pow(direction.z, 2));
	}

	template <class T>
	inline Vector3<T> Vector3<T>::Up()
	{
		return { 0, 1, 0 };
	}

	template <class T>
	inline Vector3<T> Vector3<T>::Down()
	{
		return { 0, -1, 0 };
	}
	template <class T>
	inline Vector3<T> Vector3<T>::Left()
	{
		return { -1, 0, 0 };
	}
	template <class T>
	inline Vector3<T> Vector3<T>::Right()
	{
		return { 1, 0, 0 };
	}
	template <class T>
	inline Vector3<T> Vector3<T>::Forward()
	{
		return { 0, 0, 1 };
	}
	template <class T>
	inline Vector3<T> Vector3<T>::Back()
	{
		return { 0, 0, -1 };
	}

#pragma endregion Static Functions
}