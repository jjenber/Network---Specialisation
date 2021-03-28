#pragma once
#include <cassert>
#include <initializer_list>
#pragma warning (disable: 6385) // 
#pragma warning (disable: 6386) // Buffer overrun will fail assertion.
namespace CommonUtilities
{
	template <typename T, int SIZE>
	class StaticArray
	{
	public:
		class Iterator
		{
			typedef StaticArray<T, SIZE>* ContainerPtr;
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

		StaticArray() = default;
		StaticArray(const StaticArray& aStaticArray);
		StaticArray(const std::initializer_list<T>& aInitList);
		~StaticArray() = default;
		StaticArray& operator=(const StaticArray& aStaticArray);
		inline const T& operator[](const int aIndex) const;
		inline T& operator[](const int aIndex);
		inline void Insert(const int aIndex, const T& aValue);
		inline int Size() const { return SIZE; }


	private:
		T myData[SIZE] = { };
	};

	template<typename T, int SIZE>
	inline StaticArray<T, SIZE>::StaticArray(const StaticArray& aStaticArray)
	{
		for (int i = 0; i < SIZE; i++)
		{
			myData[i] = aStaticArray[i];
		}
	}

	template<typename T, int SIZE>
	inline StaticArray<T, SIZE>::StaticArray(const std::initializer_list<T>& aInitList)
	{
		assert(aInitList.size() <= SIZE && "Initializer list has too many values.");
		for (int i = 0; i < SIZE; i++)
		{
			myData[i] = i < aInitList.size() ? *(aInitList.begin() + i) : *(aInitList.begin() + aInitList.size() - 1);
		}
	}

	template<typename T, int SIZE>
	inline StaticArray<T, SIZE>& StaticArray<T, SIZE>::operator=(const StaticArray& aStaticArray)
	{
		for (int i = 0; i < SIZE; i++)
		{
			myData[i] = aStaticArray[i];
		}
		return *this;
	}

	template<typename T, int SIZE>
	inline const T& StaticArray<T, SIZE>::operator[](const int aIndex) const
	{
		assert(aIndex >= 0 && aIndex < SIZE && "Index out of bounds");
		return myData[aIndex];
	}

	template<typename T, int SIZE>
	inline T& StaticArray<T, SIZE>::operator[](const int aIndex)
	{
		assert(aIndex >= 0 && aIndex < SIZE && "Index out of bounds");
		return myData[aIndex];
	}

	template<typename T, int SIZE>
	inline void StaticArray<T, SIZE>::Insert(const int aIndex, const T& aValue)
	{
		assert(aIndex >= 0 && aIndex < SIZE && "Index out of bounds");
		for (int i = SIZE - 1; i >= aIndex; i--)
		{
			myData[i] = myData[i - 1];
		}
		myData[aIndex] = aValue;
	}
}