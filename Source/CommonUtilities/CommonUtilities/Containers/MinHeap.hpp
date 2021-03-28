#pragma once
#include "GrowingArray.h"

namespace CommonUtilities
{
	template <class T>
	class MinHeap
	{
	public:
		MinHeap();
		MinHeap(int aReserved);

		bool IsEmpty() const;
		int GetSize() const;
		const T& GetTop() const;

		void Enqueue(const T& anElement);
		T Dequeue();

	private:
		void BubbleUp();
		void BubbleDown();
		int Left(int aIndex) const;
		int Right(int aIndex) const;
		int GetParent(int aChild) const;
		void SwapNodes(int aParent, int aChild);

		GrowingArray<T, int> myData;
	};


	template<class T>
	inline MinHeap<T>::MinHeap()
	{
		myData.Init();
	}

	template<class T>
	inline MinHeap<T>::MinHeap(int aReserved)
	{
		myData.Reserve(aReserved);
	}

	template<class T>
	inline bool MinHeap<T>::IsEmpty() const
	{
		return myData.Size() == 0;
	}

	template<class T>
	inline int MinHeap<T>::GetSize() const
	{
		return myData.Size();
	}
	template<class T>
	inline const T& MinHeap<T>::GetTop() const
	{
		return myData[0];
	}
	template<class T>
	inline void MinHeap<T>::Enqueue(const T& anElement)
	{
		myData.Add(anElement);
		BubbleUp();
	}
	template<class T>
	inline T MinHeap<T>::Dequeue()
	{
		int child = myData.Size() - 1;
		SwapNodes(0, child);

		T element = myData[child];
		myData.RemoveCyclicAtIndex(child);

		BubbleDown();
		return element;
	}

	template<class T>
	inline void MinHeap<T>::BubbleUp()
	{
		int child = myData.Size() - 1;
		int parent = GetParent(child);

		while (!(child < 0) && !(parent < 0) && myData[child] < myData[parent])
		{
			SwapNodes(parent, child);
			child = parent;
			parent = GetParent(child);
		}
	}

	template<class T>
	inline void MinHeap<T>::BubbleDown()
	{
		int parent = 0;
		int size = myData.Size();

		while (true)
		{
			int left = Left(parent);
			int right = Right(parent);
			int lowest = parent;

			if (left < size && myData[left] < myData[lowest])
			{
				lowest = left;
			}

			if (right < size && myData[right] < myData[lowest])
			{
				lowest = right;
			}

			if (lowest != parent)
			{
				SwapNodes(parent, lowest);
				parent = lowest;
			}
			else
			{
				return;
			}
		}
	}

	template<class T>
	inline int MinHeap<T>::Left(int aIndex) const
	{
		return 2 * aIndex + 1;
	}

	template<class T>
	inline int MinHeap<T>::Right(int aIndex) const
	{
		return 2 * aIndex + 2;
	}

	template<class T>
	inline int MinHeap<T>::GetParent(int aChild) const
	{
		return aChild % 2 == 0 ? static_cast<int>(aChild * 0.5f) - 1 : static_cast<int>(aChild * 0.5f);
	}

	template<class T>
	inline void MinHeap<T>::SwapNodes(int aParent, int aChild)
	{
		T element = myData[aChild];
		myData[aChild] = myData[aParent];
		myData[aParent] = element;
	}
}