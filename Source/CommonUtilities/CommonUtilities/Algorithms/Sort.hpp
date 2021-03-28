#pragma once
#include <algorithm>
#include <vector>
namespace CommonUtilities
{
	namespace Internal
	{
		template<class T>
		void QuickSort(std::vector<T>& aVector, int aFirst, int aLast)
		{
			const size_t size = aVector.size();
			int lower = aFirst + 1;
			int upper = aLast;
			std::swap(aVector[aFirst], aVector[(aFirst + aLast) / 2]);
			T bound = aVector[aFirst];
			while (lower <= upper)
			{
				while (lower < size && !(bound < aVector[lower]))
				{
					++lower;
				}
				while (bound < aVector[upper])
				{
					--upper;
				}
				if (lower < upper)
				{
					std::swap(aVector[lower++], aVector[upper--]);
				}
				else
				{
					lower++;
				}
			}
			std::swap(aVector[upper], aVector[aFirst]);
			if (aFirst < upper - 1)
			{
				QuickSort(aVector, aFirst, upper - 1);
			}
			if (upper + 1 < aLast)
			{
				QuickSort(aVector, upper + 1, aLast);
			}
		}

		template <class T>
		void MergeSort(std::vector<T>& aResult, size_t aFirst, size_t aLast)
		{
			if (aFirst < aLast)
			{
				size_t midIndex = (aFirst + aLast) / 2;
				MergeSort(aResult, aFirst, midIndex);
				MergeSort(aResult, midIndex + 1, aLast);
				Merge(aResult, aFirst, aLast);
			}
		}
		template <class T>
		void Merge(std::vector<T>& aResult, size_t aFirst, size_t aLast)
		{
			size_t midIndex = (aFirst + aLast) / 2;
			size_t i2 = aFirst;
			size_t i3 = midIndex + 1;
			std::vector<T> temp;
			temp.reserve(aLast + 1);

			while (i2 <= midIndex && i3 <= aLast)
			{
				size_t smallerIndex = aResult[i2] < aResult[i3] ? i2++ : i3++;
				temp.push_back(aResult[smallerIndex]);
			}
			while (i2 <= midIndex)
			{
				temp.push_back(aResult[i2++]);
			}
			while (i3 <= aLast)
			{
				temp.push_back(aResult[i3++]);
			}
			for (int i = 0; i < temp.size(); i++)
			{
				aResult[i + aFirst] = temp[i];
			}
			std::copy(temp.begin(), temp.end(), aResult.begin() + aFirst);
		}
	}

	template<class T>
	void SelectionSort(std::vector<T>& aVector)
	{
		auto iterator = aVector.begin();
		while (iterator != aVector.end())
		{
			auto previousItr = iterator;
			for (auto nextSelectionItr = iterator + 1; nextSelectionItr != aVector.end(); nextSelectionItr++)
			{
				if (*nextSelectionItr < *previousItr)
				{
					previousItr = nextSelectionItr;
				}
			}
			std::iter_swap(iterator, previousItr);
			++iterator;
		}
	}

	template<class T>
	void QuickSort(std::vector<T>& aVector)
	{
		size_t size = aVector.size();
		if (size < 2)
		{
			return;
		}
		int max = 0;
		for (int i = 1; i < size; i++)
		{
			if (aVector[max] < aVector[i])
			{
				max = i;
			}
		}
		std::swap(aVector[size - 1], aVector[max]);
		Internal::QuickSort(aVector, 0, static_cast<int>(size - 2));
	}

	template <class T>
	void MergeSort(std::vector<T>& aVector)
	{
		if (aVector.size() < 2) return;
		Internal::MergeSort(aVector, 0, aVector.size() - 1);
	}

	template <class T>
	void BubbleSort(std::vector<T>& aVector)
	{
		for (size_t i = 0; i < aVector.size() - 1; i++)
		{
			for (int j = static_cast<int>(aVector.size()) - 1; j > i; --j)
			{
				if (aVector[j] < aVector[size_t(j) - 1])
				{
					std::swap(aVector[j], aVector[size_t(j) - 1]);
				}
			}
		}
	}
}