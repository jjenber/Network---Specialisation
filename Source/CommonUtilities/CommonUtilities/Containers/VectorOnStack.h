#pragma once
#include <cassert>
#include <initializer_list>

#pragma warning (disable : 6385) // Reading invalid data from this->myData. VectorOnStack will assert if the user goes oout of bounds.
#pragma warning (disable : 6386) // Buffer overrun writing to this->myData. VectorOnStack will assert if the user goes oout of bounds.

namespace CommonUtilities
{
	template <typename T, unsigned int SIZE, typename SizeType = unsigned int, bool UseSafeModeFlag = true>
	class VectorOnStack
	{
	public:
		class Iterator
		{
			typedef VectorOnStack<T, SIZE, SizeType, UseSafeModeFlag>* ContainerPtr;
		public:
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;
			using iterator_category = std::random_access_iterator_tag;

			Iterator() : myPtr(nullptr), myPos(0) {}
			Iterator(ContainerPtr aPtr, int aPosition = 0) : myPtr(aPtr), myPos(aPosition) {}

			reference       operator*() { return (*myPtr)[myPos]; }
			const reference operator*() const { return (*myPtr)[myPos]; }
			pointer         operator->() { return &((*myPtr)[myPos]); }
			const pointer   operator->() const { return &((*myPtr)[myPos]); }
			reference       operator[](int aOffset) { return (*myPtr)[myPos + aOffset]; }
			const reference operator[](int aOffset) const { return (*myPtr)[myPos + aOffset]; }

			Iterator& operator++() { ++myPos; return *this; }
			Iterator& operator--() { --myPos; return *this; }
			Iterator  operator++(int) { Iterator returnVal(*this); ++myPos; return returnVal; }
			Iterator  operator--(int) { Iterator returnVal(*this); --myPos; return returnVal; }

			Iterator& operator+=(int aOffset) { myPos += aOffset; return *this; }
			Iterator& operator-=(int aOffset) { myPos -= aOffset; return *this; }

			Iterator operator+(int aOffset)   const { Iterator retVal(*this); return retVal += aOffset; }
			Iterator operator-(int aOffset)   const { Iterator retVal(*this); return retVal -= aOffset; }

			difference_type operator-(Iterator const& aOther) const { return myPos - aOther.myPos; }

			bool operator< (Iterator const& aOtherItr) const { return myPos < aOtherItr.myPos; }
			bool operator<=(Iterator const& aOtherItr) const { return myPos <= aOtherItr.myPos; }
			bool operator> (Iterator const& aOtherItr) const { return myPos > aOtherItr.myPos; }
			bool operator>=(Iterator const& aOtherItr) const { return myPos >= aOtherItr.myPos; }
			bool operator!=(const Iterator& aOtherItr) const { return myPos != aOtherItr.myPos; }
			bool operator==(const Iterator& aOtherItr) const { return myPos == aOtherItr.myPos; }
		private:
			int myPos = 0;
			ContainerPtr myPtr;
		};

		Iterator begin() { return Iterator(this); };
		Iterator end() { return Iterator(this, SIZE); };


		VectorOnStack() = default;
		VectorOnStack(const VectorOnStack& aVectorOnStack);
		VectorOnStack(const std::initializer_list<T>& aInitList);
		
		~VectorOnStack() = default;

		VectorOnStack& operator=(const VectorOnStack& aVectorOnStack);
		inline const T& operator[](const SizeType aIndex) const;
		inline T& operator[](const SizeType aIndex);
		
		inline void Add(const T& aValue);
		inline void Insert(const SizeType aIndex, const T& aValue);
		inline void RemoveCyclic(const T& aValue);
		inline void RemoveCyclicAtIndex(const SizeType aIndex);
		inline void Clear();
		__forceinline SizeType Size() const;
		__forceinline constexpr int Capacity() const;

	private:
		T myData[SIZE] = { };
		SizeType mySize = 0;
	};

	template<typename T, unsigned int SIZE, typename SizeType, bool UseSafeModeFlag>
	inline VectorOnStack<T, SIZE, SizeType, UseSafeModeFlag>::VectorOnStack(const VectorOnStack& aVectorOnStack)
	{
		if (UseSafeModeFlag)
		{
			for (SizeType i = 0; i < aVectorOnStack.mySize; i++)
			{
				myData[i] = aVectorOnStack[i];
			}
		}
		else
		{
			std::memcpy(myData, aVectorOnStack.myData, sizeof(T) * aVectorOnStack.mySize);
		}
		mySize = aVectorOnStack.mySize;
	}

	template<typename T, unsigned int SIZE, typename SizeType, bool UseSafeModeFlag>
	inline VectorOnStack<T, SIZE, SizeType, UseSafeModeFlag>::VectorOnStack(const std::initializer_list<T>& aInitList)
	{
		assert(aInitList.size() <= SIZE && "Initializer list has too many values.");
		if (UseSafeModeFlag)
		{
			for (int i = 0; i < aInitList.size(); i++)
			{
				myData[i] = *(aInitList.begin() + i);
			}
		}
		else
		{
			std::memcpy(myData, aInitList.begin(), sizeof(T) * aInitList.size());
		}
		mySize = static_cast<SizeType>(aInitList.size());
	}

	template<typename T, unsigned int SIZE, typename SizeType, bool UseSafeModeFlag>
	inline VectorOnStack<T, SIZE, SizeType, UseSafeModeFlag>& VectorOnStack<T, SIZE, SizeType, UseSafeModeFlag>::operator=(const VectorOnStack& aVectorOnStack)
	{
		if (UseSafeModeFlag)
		{
			for (SizeType i = 0; i < aVectorOnStack.mySize; i++)
			{
				myData[i] = aVectorOnStack[i];
			}
		}
		else
		{
			std::memcpy(myData, aVectorOnStack.myData, sizeof(T) * SIZE);
		}
		mySize = aVectorOnStack.mySize;
		return *this;
	}

	template<typename T, unsigned int SIZE, typename SizeType, bool UseSafeModeFlag>
	inline const T& VectorOnStack<T, SIZE, SizeType, UseSafeModeFlag>::operator[](const SizeType aIndex) const
	{
		assert(aIndex >= 0 && aIndex < mySize && "Index out of bounds");
		return myData[aIndex];
	}

	template<typename T, unsigned int SIZE, typename SizeType, bool UseSafeModeFlag>
	inline T& VectorOnStack<T, SIZE, SizeType, UseSafeModeFlag>::operator[](const SizeType aIndex)
	{
		assert(aIndex >= 0 && aIndex < mySize && "Index out of bounds");
		return myData[aIndex];
	}

	template<typename T, unsigned int SIZE, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<T, SIZE, SizeType, UseSafeModeFlag>::Add(const T& aObject)
	{
		assert(mySize < SIZE && "Vector is full.");
		myData[mySize++] = aObject;
	}

	template<typename T, unsigned int SIZE, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<T, SIZE, SizeType, UseSafeModeFlag>::Insert(const SizeType aIndex, const T& aValue)
	{
		assert(aIndex >= 0 && (aIndex <= mySize) && "Index is out of bounds.");
		assert(mySize < SIZE && "Vector is full.");
		if (UseSafeModeFlag)
		{
			for (SizeType i = mySize; i > aIndex; i--)
			{
				myData[i] = myData[i - SizeType(1)];
			}
		}
		else
		{
			const size_t memSize = sizeof(T) * (static_cast<size_t>(mySize) - aIndex);
			std::memcpy(&myData[aIndex + 1], &myData[aIndex], memSize);
		}
		myData[aIndex] = aValue;
		mySize = mySize < SIZE ? mySize + 1 : mySize;
	}

	template<typename T, unsigned int SIZE, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<T, SIZE, SizeType, UseSafeModeFlag>::RemoveCyclic(const T& aValue)
	{
		assert(mySize > 0 && "Vector is already empty.");

		for (SizeType i = 0; i < mySize; i++)
		{
			if (myData[i] == aValue)
			{
				myData[i] = myData[mySize-- - 1];
				break;
			}
			if (i == mySize - 1)
			{
				assert(false && "Value not found.");
			}
		}
	}

	template<typename T, unsigned int SIZE, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<T, SIZE, SizeType, UseSafeModeFlag>::RemoveCyclicAtIndex(const SizeType aIndex)
	{
		assert(aIndex >= 0 && aIndex < mySize && "Index out of bounds");
		myData[aIndex] = myData[mySize-- - 1];
	}

	template<typename T, unsigned int SIZE, typename SizeType, bool UseSafeModeFlag>
	inline void VectorOnStack<T, SIZE, SizeType, UseSafeModeFlag>::Clear()
	{
		mySize = 0;
	}

	template<typename T, unsigned int SIZE, typename SizeType, bool UseSafeModeFlag>
	__forceinline SizeType VectorOnStack<T, SIZE, SizeType, UseSafeModeFlag>::Size() const
	{
		return mySize;
	}
	template<typename T, unsigned int SIZE, typename SizeType, bool UseSafeModeFlag>
	__forceinline constexpr int VectorOnStack<T, SIZE, SizeType, UseSafeModeFlag>::Capacity() const
	{
		return SIZE;
	}
}
