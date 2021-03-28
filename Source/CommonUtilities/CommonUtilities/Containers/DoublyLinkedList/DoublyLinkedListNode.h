#pragma once

namespace CommonUtilities
{
	template <class T>
	class DoublyLinkedListNode
	{
		template<class U> friend class DoublyLinkedList;
	public:
		DoublyLinkedListNode<T>(const DoublyLinkedListNode<T>&) = delete;
		DoublyLinkedListNode<T>& operator=(const DoublyLinkedListNode<T>&) = delete;

		const T& GetValue() const;
		T& GetValue();
		DoublyLinkedListNode<T>* GetNext() const;
		DoublyLinkedListNode<T>* GetPrevious() const;

	private:
		DoublyLinkedListNode(const T& aValue);
		DoublyLinkedListNode(T& aValue);

		T myValue;
		DoublyLinkedListNode<T>* myNext = nullptr;
		DoublyLinkedListNode<T>* myPrevious = nullptr;
	};

	template <class T>
	DoublyLinkedListNode<T>::DoublyLinkedListNode(const T& aValue)
	{
		myValue = aValue;
	}
	template <class T>
	DoublyLinkedListNode<T>::DoublyLinkedListNode(T& aValue)
	{
		myValue = aValue;
	}

	template<class T>
	inline const T& DoublyLinkedListNode<T>::GetValue() const
	{
		return myValue;
	}

	template<class T>
	inline T& DoublyLinkedListNode<T>::GetValue()
	{
		return myValue;
	}

	template<class T>
	inline DoublyLinkedListNode<T>* DoublyLinkedListNode<T>::GetNext() const
	{
		return myNext;
	}

	template<class T>
	inline DoublyLinkedListNode<T>* DoublyLinkedListNode<T>::GetPrevious() const
	{
		return myPrevious;
	}
}
