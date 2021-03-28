#pragma once
#include "GrowingArray.h"
namespace CommonUtilities
{
	template <class T>
	class Stack
	{
	public:
		Stack();

		int GetSize() const;
		bool Empty() const;
		const T& GetTop() const;

		void Push(const T& aValue);
		T Pop();


	private:
		GrowingArray<T, int> myData;
	};

	template<class T>
	inline Stack<T>::Stack()
	{
		myData.Init();
	}
	template<class T>
	inline int Stack<T>::GetSize() const
	{
		return myData.mySize;
	}
	template<class T>
	inline bool Stack<T>::Empty() const
	{
		return myData.mySize == 0;
	}
	template<class T>
	inline const T& Stack<T>::GetTop() const
	{
		return myData[myData.Size() - 1];
	}
	template<class T>
	inline void Stack<T>::Push(const T& aValue)
	{
		myData.Add(aValue);
	}
	template<class T>
	inline T Stack<T>::Pop()
	{
		assert(!Empty() && "Stack is empty");
		auto index = myData.mySize - 1;
		T value = myData[index];
		myData.RemoveCyclicAtIndex(index);
		return value;
	}
}
