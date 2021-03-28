#pragma once
#include "GrowingArray.h"

namespace CommonUtilities
{
	template <class T>
	class Heap
	{
	public:
		Heap();

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
	inline Heap<T>::Heap()
	{
		myData.Init();
	}

	template<class T>
	inline int Heap<T>::GetSize() const
	{
		return myData.Size();
	}
	template<class T>
	inline const T& Heap<T>::GetTop() const
	{
		return myData[0];
	}
	template<class T>
	inline void Heap<T>::Enqueue(const T& anElement)
	{
		myData.Add(anElement);
		BubbleUp();
	}
	template<class T>
	inline T Heap<T>::Dequeue()
	{
		int child = myData.Size() - 1;
		SwapNodes(0, child);

		T element = myData[child];
		myData.RemoveCyclicAtIndex(child);

		BubbleDown();
		return element;
	}

	template<class T>
	inline void Heap<T>::BubbleUp()
	{
		int child = myData.Size() - 1;
		int parent = GetParent(child);

		while (!(child < 0) && !(parent < 0) && myData[parent] < myData[child])
		{
			SwapNodes(parent, child);
			child = parent;
			parent = GetParent(child);
		}
	}

	template<class T>
	inline void Heap<T>::BubbleDown()
	{
		int parent = 0;
		int size = myData.Size();
	
		while (true)
		{
			int left  = Left(parent);
			int right = Right(parent);
			int largest = parent;

			if (left < size && myData[largest] < myData[left])
			{
				largest = left;
			}

			if (right < size && myData[largest] < myData[right])
			{
				largest = right;
			}

			if (largest != parent)
			{
				SwapNodes(parent, largest);
				parent = largest;
			}
			else
			{
				return;
			}
		}
	}
	
	template<class T>
	inline int Heap<T>::Left(int aIndex) const
	{
		return 2 * aIndex + 1;
	}
	
	template<class T>
	inline int Heap<T>::Right(int aIndex) const
	{
		return 2 * aIndex + 2;
	}

	template<class T>
	inline int Heap<T>::GetParent(int aChild) const
	{
		return aChild % 2 == 0 ? static_cast<int>(aChild * 0.5f) - 1 : static_cast<int>(aChild * 0.5f);
	}

	template<class T>
	inline void Heap<T>::SwapNodes(int aParent, int aChild)
	{
		T element = myData[aChild];
		myData[aChild]  = myData[aParent];
		myData[aParent] = element;
	}
}