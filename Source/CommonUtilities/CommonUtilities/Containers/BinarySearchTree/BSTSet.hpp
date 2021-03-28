#pragma once
#include "BSTSetNode.h"
#include <cassert>
namespace CommonUtilities
{
	template <class T>
	class BSTSet
	{
	public:
		~BSTSet();
		size_t				 GetSize() const;
		bool				 HasElement(const T& aValue) const;
		bool				 IsEmpty() const;
		void				 Insert(const T& aValue);
		void				 Remove(const T& aValue);
		void				 Clear();
		void				 DSWBalance();
		const BSTSetNode<T>* GetRoot() const;

	private:
		bool				 FindValue(BSTSetNode<T>* aNode, const T& aValue) const;
		void				 CreateBackbone(BSTSetNode<T>* aRoot);
		void				 CreatePerfectTree();
		void				 RotateRight(BSTSetNode<T>* aGrandParent, BSTSetNode<T>* aParent, BSTSetNode<T>* aChild);
		void				 RotateLeft(BSTSetNode<T>* aGrandParent, BSTSetNode<T>* aParent, BSTSetNode<T>* aChild);
		void				 Compress(BSTSetNode<T>* aNode, int aCount);
		BSTSetNode<T>*		 FindNode(const T& aValue);
		BSTSetNode<T>*		 RemoveRecursive(BSTSetNode<T>* aNode, const T& aValue);

		
		BSTSetNode<T>*		 myRoot = nullptr;
		size_t				 mySize = 0;
	};

	template <class T> using Node = BSTSetNode<T>;

	template <class T>
	BSTSet<T>::~BSTSet()
	{
		Clear();
	}
	template<class T>
	inline bool BSTSet<T>::HasElement(const T& aValue) const
	{
		assert(myRoot != nullptr && "Tree is empty.");
		return FindValue(myRoot, aValue);
	}
	
	template<class T>
	size_t BSTSet<T>::GetSize() const
	{
		return mySize;
	}

	template<class T>
	inline void BSTSet<T>::Insert(const T& aValue)
	{
		Node<T>* itr = myRoot;
		Node<T>* previous = nullptr;

		while (itr != nullptr)
		{
			previous = itr;
			if (aValue < itr->myElement)
			{
				itr = itr->myLeftChild;
			}
			else if (itr->myElement < aValue)
			{
				itr = itr->myRightChild;
			}
			else
			{
				return;
			}
		}
		if (myRoot == nullptr)
		{
			myRoot = new Node<T>(aValue);
		}
		else if (aValue < previous->myElement)
		{
			previous->myLeftChild = new Node<T>(aValue);
		}
		else
		{
			previous->myRightChild = new Node<T>(aValue);
		}
		++mySize;
	}

	template<class T>
	Node<T>* BSTSet<T>::FindNode(const T& aValue)
	{
		Node<T>* itr = myRoot;
		while (itr != nullptr)
		{
			if (aValue < itr->myElement)
			{
				itr = itr->myLeftChild;
			}
			else if (itr->myElement < aValue)
			{
				itr = itr->myRightChild;
			}
			else // value found
			{
				return itr;
			}
		}
		return itr;
	}

	template<class T>
	inline void BSTSet<T>::Remove(const T& aValue)
	{
		myRoot = RemoveRecursive(myRoot, aValue);
	}
	template<class T>
	Node<T>* BSTSet<T>::RemoveRecursive(BSTSetNode<T>* aNode, const T& aValue)
	{
		if (aNode == nullptr)
			return nullptr;

		if (aValue < aNode->myElement)
		{
			aNode->myLeftChild = RemoveRecursive(aNode->myLeftChild, aValue);
		}
		else if (aNode->myElement < aValue)
		{
			aNode->myRightChild = RemoveRecursive(aNode->myRightChild, aValue);
		}
		else
		{
			if (aNode->myLeftChild == nullptr && aNode->myRightChild == nullptr)
			{
				delete aNode;
				aNode = nullptr;
				--mySize;
			}
			else if (aNode->myLeftChild == nullptr)
			{
				Node<T>* temp = aNode;
				aNode = aNode->myRightChild;
				temp->myLeftChild = nullptr;
				temp->myRightChild = nullptr;
				delete temp;
				--mySize;
			}
			else if (aNode->myRightChild == nullptr)
			{
				Node<T>* temp = aNode;
				aNode = aNode->myLeftChild;
				temp->myLeftChild = nullptr;
				temp->myRightChild = nullptr;
				delete temp;
				--mySize;
			}
			else
			{
				Node<T>* temp = aNode->myRightChild;
				while (temp->myLeftChild != nullptr)
				{
					temp = temp->myLeftChild;
				}
				aNode->myElement = temp->myElement;
				aNode->myRightChild = RemoveRecursive(aNode->myRightChild, temp->myElement);
			}
		}
		return aNode;
	}

	template<class T>
	inline void BSTSet<T>::Clear()
	{
		if (myRoot != nullptr)
		{
			delete myRoot;
			myRoot = nullptr;
		}
		mySize = 0;
	}

	template<class T>
	inline bool BSTSet<T>::IsEmpty() const
	{
		return myRoot == nullptr;
	}

	template<class T>
	inline void BSTSet<T>::DSWBalance()
	{
		CreateBackbone(myRoot);
		CreatePerfectTree();
	}

	template<class T>
	inline const Node<T>* BSTSet<T>::GetRoot() const
	{
		return myRoot;
	}

#pragma region Private
	template<class T>
	inline bool BSTSet<T>::FindValue(BSTSetNode<T>* aNode, const T& aValue) const
	{
		while (aNode != nullptr)
		{
			bool isLower =  (aValue < aNode->myElement);
			bool isHigher = (aNode->myElement < aValue);
			if (!isLower && !isHigher)
			{
				return true;
			}
			else if (isLower)
			{
				aNode = aNode->myLeftChild;
			}
			else if (isHigher)
			{
				aNode = aNode->myRightChild;
			}
		}
		return false;
	}

	template<class T>
	inline void BSTSet<T>::CreateBackbone(BSTSetNode<T>* aRoot)
	{
		Node<T>* temp       = aRoot;
		Node<T>* tempParent = nullptr;
		while (temp != nullptr)
		{
			Node<T>* child = temp->myLeftChild;
			if (child != nullptr)
			{
				
				RotateRight(tempParent, temp, child);
				temp = child;
			}
			else
			{
				tempParent = temp;
				temp = temp->myRightChild;
			}
		}
	}
	template <class T>
	inline void BSTSet<T>::RotateRight(BSTSetNode<T>* aGrandParent, BSTSetNode<T>* aParent, BSTSetNode<T>* aChild)
	{
		if (aGrandParent != nullptr)
		{
			aGrandParent->myRightChild = aChild;
		}
		else
		{
			myRoot = aChild;
		}
		aParent->myLeftChild = aChild->myRightChild;
		aChild->myRightChild = aParent;
	}

	template <class T>
	inline void BSTSet<T>::RotateLeft(BSTSetNode<T>* aGrandParent, BSTSetNode<T>* aParent, BSTSetNode<T>* aChild)
	{
		if (aGrandParent != nullptr)
		{
			aGrandParent->myRightChild = aChild;
		}
		else
		{
			myRoot = aChild;
		}
		aParent->myRightChild = aChild->myLeftChild;
		aChild->myLeftChild = aParent;
	}

	template<class T>
	inline void BSTSet<T>::Compress(BSTSetNode<T>* aNode, int aCount)
	{
		Node<T>* tempParent = nullptr;
		Node<T>* temp = aNode;

		for (int i = 0; i < aCount; i++)
		{
			if (temp->myRightChild != nullptr)
			{
				Node<T>* child = temp->myRightChild;
				RotateLeft(tempParent, temp, temp->myRightChild);
				tempParent = child;
				temp = tempParent->myRightChild;
			}
			else
			{
				break;
			}
		}

	}


	template <class T>
	inline void BSTSet<T>::CreatePerfectTree()
	{
		int size = static_cast<int>(mySize);
		int exp  = static_cast<int>(floor(log2(size + 1)));
		int m    = static_cast<int>(pow(2, exp) - 1);
		
		Compress(myRoot, size - m);

		while (m > 1)
		{
			Compress(myRoot, m /= 2);
		}
	}
#pragma endregion
}
