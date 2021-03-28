#pragma once

namespace CommonUtilities
{
	template <class T>
	class BSTSetNode
	{
		template <class U> friend class BSTSet;
	public:
		BSTSetNode(const BSTSetNode& aCopy) = delete;
		BSTSetNode& operator=(const BSTSetNode& aCopy) = delete;
		
		const T& GetData() const;
		const BSTSetNode<T>* GetLeft() const;
		const BSTSetNode<T>* GetRight() const;

		~BSTSetNode();
	private:
		BSTSetNode() = default;
		BSTSetNode(const T& aValue);

		T myElement;
		BSTSetNode* myLeftChild = nullptr;
		BSTSetNode* myRightChild = nullptr;
	};

	template <class T>
	BSTSetNode<T>::BSTSetNode(const T& aValue) 
	{
		myElement = aValue; 
	}
	template<class T>
	inline const T& BSTSetNode<T>::GetData() const
	{
		return myElement;
	}
	template<class T>
	inline const BSTSetNode<T>* BSTSetNode<T>::GetLeft() const
	{
		return myLeftChild;
	}
	template<class T>
	inline const BSTSetNode<T>* BSTSetNode<T>::GetRight() const
	{
		return myRightChild;
	}

	template <class T>
	BSTSetNode<T>::~BSTSetNode()
	{
		if (myLeftChild != nullptr)
		{
			delete myLeftChild;
			myLeftChild = nullptr;
		}
		if (myRightChild != nullptr)
		{
			delete myRightChild;
			myRightChild = nullptr;
		}
	}
}