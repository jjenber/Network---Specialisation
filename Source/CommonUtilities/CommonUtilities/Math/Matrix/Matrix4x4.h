#pragma once
#include <assert.h>
#include <initializer_list>
#include "../Vector.h"

namespace CommonUtilities
{
	template<typename T>
	class Matrix4x4
	{
	public:
		Matrix4x4<T>();
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>(std::initializer_list<T> aList);

		Matrix4x4<T>& operator=(const Matrix4x4<T>& aMatrix);

		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;
		bool operator==(const Matrix4x4<T>& aMatrix) const;
		bool operator!=(const Matrix4x4<T>& aMatrix) const;

		Vector4<T> operator*(const Vector4<T>& aVector) const;
		Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrix) const;
		Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrix) const;
		Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix) const;
		Matrix4x4<T> operator*(const T& aScalar) const;
		Matrix4x4<T>& operator+=(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>& operator-=(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>& operator*=(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>& operator*=(const T& aScalar);

		Vector3<T> GetForward() const;
		Vector3<T> GetUp() const;
		Vector3<T> GetRight() const;
		Vector3<T> GetTranslation() const;

		void SetTranslation(const Vector3<T>& aPosition);
		void Translate(const Vector3<T>& aDirection);
		void Transpose();

		static Matrix4x4<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(T aAngleInRadians);
		static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose);
		static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform);
		Matrix4x4<T> GetFastInverse();
		
		static const size_t myLength = 16;
		T myData[16];
	};
	typedef Matrix4x4<float> Matrix44f;


	template<class T>
	Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		Vector4<T> result;
		result.x = (aMatrix(1, 1) * aVector.x) + (aMatrix(2, 1) * aVector.y) + (aMatrix(3, 1) * aVector.z) + (aMatrix(4, 1) * aVector.w);
		result.y = (aMatrix(1, 2) * aVector.x) + (aMatrix(2, 2) * aVector.y) + (aMatrix(3, 2) * aVector.z) + (aMatrix(4, 2) * aVector.w);
		result.z = (aMatrix(1, 3) * aVector.x) + (aMatrix(2, 3) * aVector.y) + (aMatrix(3, 3) * aVector.z) + (aMatrix(4, 3) * aVector.w);
		result.w = (aMatrix(1, 4) * aVector.x) + (aMatrix(2, 4) * aVector.y) + (aMatrix(3, 4) * aVector.z) + (aMatrix(4, 4) * aVector.w);
		return result;
	}

#pragma region Constructors
	template <typename T> Matrix4x4<T>::Matrix4x4() 
	{
		std::memset(myData, 0, myLength * sizeof(T));
		myData[0] = 1;
		myData[5] = 1;
		myData[10] = 1;
		myData[15] = 1;
	}
	template <typename T> Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix)
	{
		std::memcpy(myData, aMatrix.myData, sizeof(T) * myLength);
	}

	template<typename T> inline Matrix4x4<T>::Matrix4x4(std::initializer_list<T> aList)
	{
		assert(aList.size() <= myLength && "Initializer list contains too many values.");
		std::memcpy(myData, aList.begin(), sizeof(T) * aList.size());
		if (aList.size() < myLength)
		{
			std::fill(myData + aList.size(), myData + myLength, myData[aList.size() - 1]);
		}
	}
#pragma endregion Constructors

#pragma region Operators

	template<typename T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator=(const Matrix4x4<T>& aMatrix)
	{
		std::memcpy(myData, aMatrix.myData, sizeof(T) * myLength);
		return *this;
	}

	// Rows and Columns start at 1.
	template<typename T> inline T& Matrix4x4<T>::operator()(const int aRow, const int aColumn)
	{
		assert(aRow > 0 && aRow < 5 && aColumn > 0 && aColumn < 5 && "Argument out of bounds");
		return myData[(aRow - 1) * 4 + (aColumn - 1)];
	}

	// Rows and Columns start at 1.
	template<typename T> inline const T& Matrix4x4<T>::operator()(const int aRow, const int aColumn) const
	{
		assert(aRow > 0 && aRow < 5 && aColumn > 0 && aColumn < 5 && "Argument out of bounds");
		return myData[(aRow - 1) * 4 + (aColumn - 1)];
	}

	template<typename T>
	inline bool Matrix4x4<T>::operator==(const Matrix4x4<T>& aMatrix) const
	{
		for (int i = 0; i < myLength; i++)
		{
			if (myData[i] != aMatrix.myData[i])
			{
				return false;
			}
		}
		return true;
	}

	template<typename T>
	inline bool Matrix4x4<T>::operator!=(const Matrix4x4<T>& aMatrix) const
	{
		return !operator==(aMatrix);
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4<T>& aMatrix) const
	{
		Matrix4x4<T> result{ *this };
		return result += aMatrix;
	}

	template<typename T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator+=(const Matrix4x4<T>& aMatrix)
	{
		for (auto i = 0; i < myLength; i++)
		{
			myData[i] += aMatrix.myData[i];
		}
		return *this;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4<T>& aMatrix) const
	{
		Matrix4x4<T> result{ *this };
		return result -= aMatrix;
	}

	template<typename T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator-=(const Matrix4x4<T>& aMatrix)
	{
		for (auto i = 0; i < myLength; i++)
		{
			myData[i] -= aMatrix.myData[i];
		}
		return *this;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& aMatrix) const
	{
		Matrix4x4<T> result{ *this };
		return result *= aMatrix;
	}

	template<typename T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4<T>& aMatrix)
	{
		Matrix4x4<T> result;
		for (int i = 1; i <= 4; i++)
		{
			for (auto j = 1; j <= 4; j++)
			{
				T product{ 0 };
				for (auto k = 1; k <= 4; k++)
				{
					product += this->operator()(i, k) * aMatrix(k, j);
				}
				result(i, j) = product;
			}
		}
		std::memcpy(this->myData, result.myData, sizeof(T) * myLength);
		return *this;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const T& aScalar) const
	{
		Matrix4x4<T> result{ *this };
		return result *= aScalar;
	}
	template<typename T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator*=(const T& aScalar)
	{
		for (int i = 0; i < myLength; i++)
		{
			myData[i] *= aScalar;
		}
		return *this;
	}

	template<typename T>
	inline Vector4<T> Matrix4x4<T>::operator*(const Vector4<T>& aVector) const
	{
		Vector4<T> result;
		result.x = (myData[0] * aVector.x) + (myData[4] * aVector.y) + (myData[8] * aVector.z) + (myData[12] * aVector.w);
		result.y = (myData[1] * aVector.x) + (myData[5] * aVector.y) + (myData[9] * aVector.z) + (myData[13] * aVector.w);
		result.z = (myData[2] * aVector.x) + (myData[6] * aVector.y) + (myData[10] * aVector.z) + (myData[14] * aVector.w);
		result.w = (myData[3] * aVector.x) + (myData[7] * aVector.y) + (myData[11] * aVector.z) + (myData[15] * aVector.w);
		return result;
	}
#pragma endregion Operators
	template<typename T>
	inline Vector3<T> Matrix4x4<T>::GetForward() const
	{
		Matrix4x4<T> rotation = *this;
		rotation(4, 1) = 0;
		rotation(4, 2) = 0;
		rotation(4, 3) = 0;
		auto result = rotation * Vector4<T> { 0, 0, 1.f, 1.f };
		return { result.x, result.y, result.z };
	}

	template<typename T>
	inline Vector3<T> Matrix4x4<T>::GetUp() const
	{
		Matrix4x4<T> rotation = *this;
		rotation(4, 1) = 0;
		rotation(4, 2) = 0;
		rotation(4, 3) = 0;
		auto result = rotation * Vector4<T> { 0, 1.f, 0, 1.f };
		return { result.x, result.y, result.z };
	}

	template<typename T>
	inline Vector3<T> Matrix4x4<T>::GetRight() const
	{
		Matrix4x4<T> rotation = *this;
		rotation(4, 1) = 0;
		rotation(4, 2) = 0;
		rotation(4, 3) = 0;
		auto result = rotation * Vector4<T> { 1.f, 0, 0, 1.f };
		return { result.x, result.y, result.z };
	}

	template<typename T>
	inline void Matrix4x4<T>::SetTranslation(const Vector3<T>& aPosition)
	{
		myData[12] = aPosition.x;
		myData[13] = aPosition.y;
		myData[14] = aPosition.z;
	}
	template<typename T>
	inline Vector3<T> Matrix4x4<T>::GetTranslation() const
	{
		return { myData[12], myData[13], myData[14] };
	}

	template<typename T>
	void Matrix4x4<T>::Translate(const Vector3<T>& aDirection)
	{
		myData[12] += aDirection.x;
		myData[13] += aDirection.y;
		myData[14] += aDirection.z;
	}

	template<typename T>
	void Matrix4x4<T>::Transpose()
	{
		Matrix4x4<T> result;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result.myData[i + j * 4] = myData[j + i * 4];
			}
		}
		memcpy(myData, result.myData, sizeof(T) * 16);
	}

#pragma region Static Functions

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);
		return
		{
			T(1), 0,  0,  0,
			0,  cos, sin, 0,
			0, -sin, cos, 0,
			0,  0,  0,  T(1)
		};
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);
		return
		{
			cos, 0, -sin, 0,
			0, T(1), 0,   0,
			sin, 0,  cos, 0,
			0,   0,  0, T(1)
		};
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);
		return
		{
			cos,  sin, 0, 0,
			-sin, cos, 0, 0,
			0,    0, T(1),0,
			0,    0, 0, T(1)
		};
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T>& aMatrixToTranspose)
	{
		Matrix4x4<T> result;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result.myData[i + j * 4] = aMatrixToTranspose.myData[j + i * 4];
			}
		}
		return result;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aTransform)
	{
		const CommonUtilities::Matrix4x4<T> rt = CommonUtilities::Matrix4x4<T>::Transpose(aTransform);
		const Vector4<T> translation = rt * Vector4<T>{ -aTransform.myData[12], -aTransform.myData[13], -aTransform.myData[14], T(1) };
		return
		{
			rt.myData[0], rt.myData[1], rt.myData[2], 0,
			rt.myData[4], rt.myData[5], rt.myData[6], 0,
			rt.myData[8], rt.myData[9], rt.myData[10], 0,
			translation.x, translation.y, translation.z, T(1)
		};
	}
#pragma endregion Static Functions
	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::GetFastInverse()
	{
		return Matrix4x4<T>::GetFastInverse(*this);
	};
}