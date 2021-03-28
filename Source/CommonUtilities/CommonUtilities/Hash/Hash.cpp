#include "Hash.h"
//#include "Hash/xxhash/xxhash.hpp"
#include "Hash/xxhash/xxhash_cx.h"

namespace CommonUtilities
{
   /* unsigned int Hash::Get(const std::string& aString, const unsigned int aSeed)
    {
        return xxh::xxhash<32>(aString, aSeed);
    }*/

    // Compile time evaluation
    inline constexpr unsigned int Hash::Get(const char* aString, const size_t aLength, const unsigned int aSeed)
    {
        return xxhash::xxh32(aString, aLength, aSeed);
    }
}
