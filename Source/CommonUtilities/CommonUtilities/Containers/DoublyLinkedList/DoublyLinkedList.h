#pragma once
#include "DoublyLinkedListNode.h"
#include <cassert>

namespace CommonUtilities
{
	template <class T>
	class DoublyLinkedList
	{
	public:
		typedef DoublyLinkedListNode<T> Node;

		DoublyLinkedList();
		~DoublyLinkedList();

		int GetSize() const;
		Node* GetFirst();
		Node* GetLast();

		void InsertFirst(const T& aValue);
		void InsertLast(const T& aValue);
		void InsertBefore(Node* aNode, const T& aValue);
		void InsertAfter(Node* aNode, const T& aValue);
		void Remove(Node* aNode);
		Node* FindFirst(const T& aValue);
		Node* FindLast(const T& aValue);
		bool RemoveFirst(const T& aValue);
		bool RemoveLast(const T& aValue);

	private:
		int mySize;
		Node* myFirst;
		Node* myLast;
	};

	template<class T>
	DoublyLinkedList<T>::DoublyLinkedList() 
		: myFirst(nullptr), myLast(nullptr), mySize(0)
	{}

	template<class T>
	DoublyLinkedList<T>::~DoublyLinkedList()
	{
		while (myFirst != nullptr)
		{
			DoublyLinkedListNode<T>* temp = myFirst;
			myFirst = myFirst->GetNext();
			delete temp;
		}
	}

	template<class T>
	inline int DoublyLinkedList<T>::GetSize() const
	{
		return mySize;
	}
	template<class T>
	inline DoublyLinkedListNode<T>* DoublyLinkedList<T>::GetFirst()
	{
		return myFirst;
	}
	template<class T>
	inline DoublyLinkedListNode<T>* DoublyLinkedList<T>::GetLast()
	{
		return myLast;
	}
	template<class T>
	inline void DoublyLinkedList<T>::InsertFirst(const T& aValue)
	{
		Node* newNode = new Node(aValue);
		if (mySize > 0)
		{
			myFirst->myPrevious = newNode;
			newNode->myNext = myFirst;
		}
		else
		{
			myLast = newNode;
		}
		myFirst = newNode;
		++mySize;
	}

	template<class T>
	inline void DoublyLinkedList<T>::InsertLast(const T& aValue)
	{
		Node* newNode = new Node(aValue);
		if (mySize > 0)
		{
			myLast->myNext = newNode;
			newNode->myPrevious = myLast;
		}
		else
		{
			myFirst = newNode;
		}
		myLast = newNode;
		++mySize;
	}

	template<class T>
	inline void DoublyLinkedList<T>::InsertBefore(Node* aNode, const T& aValue)
	{
		assert(mySize > 0 && "List is empty");
		assert(aNode != nullptr && "Argument aNode is nullptr");
		
		if (aNode == myFirst)
		{
			InsertFirst(aValue);
			return;
		}

		Node* newNode = new Node(aValue);
		Node* itr     = myFirst;

		while (itr != nullptr)
		{
			if (aNode == itr)
			{
				newNode->myNext = aNode;
				newNode->myPrevious = aNode->myPrevious;

				aNode->myPrevious->myNext = newNode;
				aNode->myPrevious = newNode;
				++mySize;
				return;
			}
			itr = itr->myNext;
		}
		assert(itr != nullptr && "Node was not found!");
	}

	template<class T>
	inline void DoublyLinkedList<T>::InsertAfter(Node* aNode, const T& aValue)
	{
		assert(mySize > 0 && "List is empty");
		assert(aNode != nullptr && "Argument aNode is nullptr");

		
		if (aNode == myLast)
		{
			InsertLast(aValue);
			return;
		}

		Node* itr = myFirst;
		Node* newNode = new Node(aValue);
		while (itr != nullptr)
		{
			if (aNode == itr)
			{
				itr->myNext->myPrevious = newNode;
				newNode->myNext         = itr->myNext;
				aNode->myNext           = newNode;
				newNode->myPrevious     = aNode;
				++mySize;
				return;
			}
			itr = itr->myNext;
		}
		assert(itr != nullptr && "Node was not found!");
	}

	template<class T>
	inline void DoublyLinkedList<T>::Remove(Node* aNode)
	{
		assert(aNode != nullptr && "Argument can not be nullptr");
		Node* itr = myFirst;
		while (itr != nullptr)
		{
			if (itr == aNode)
			{
				Node* prev = itr->myPrevious;
				Node* next = itr->myNext;
				if (prev != nullptr)
				{
					prev->myNext = next;
					if (itr == myLast)
					{
						myLast = prev;
					}
				}
				if (next != nullptr)
				{
					next->myPrevious = prev;
					if (itr == myFirst)
					{
						myFirst = next;
					}
				}
				--mySize;
				delete itr;
				if (mySize == 0)
				{
					myFirst = nullptr;
					myLast = nullptr;
				}
				return;
			}
			itr = itr->myNext;
		}
	}

	template <class T>
	DoublyLinkedListNode<T>* DoublyLinkedList<T>::FindFirst(const T& aValue)
	{
		Node* temp = myFirst;
		while (temp != nullptr)
		{
			if (temp->myValue == aValue)
			{
				return temp;
			}
			temp = temp->myNext;
		}
		return nullptr;
	}

	template<class T>
	inline DoublyLinkedListNode<T>* DoublyLinkedList<T>::FindLast(const T& aValue)
	{
		Node* temp = myLast;
		while (temp != nullptr)
		{
			if (temp->myValue == aValue)
			{
				return temp;
			}
			temp = temp->myPrevious;
		}
		return nullptr;
	}

	template<class T>
	inline bool DoublyLinkedList<T>::RemoveFirst(const T& aValue)
	{
		auto node = FindFirst(aValue);
		if (node != nullptr)
		{
			Remove(node);
			return true;
		}
		return false;
	}

	template<class T>
	inline bool DoublyLinkedList<T>::RemoveLast(const T& aValue)
	{
		auto node = FindLast(aValue);
		if (node != nullptr)
		{
			Remove(node);
			return true;
		}
		return false;
	}

}

