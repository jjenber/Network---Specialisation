#pragma once
#include <string>
namespace CommonUtilities
{
	class Hash
	{
		constexpr static unsigned int ourSeed = 132134;
	public:
		static unsigned int Get(const std::string& aString, const unsigned int aSeed = ourSeed);
		static constexpr unsigned int Get(const char* aString, const size_t aLength, const unsigned int aSeed = ourSeed);
	};
}
