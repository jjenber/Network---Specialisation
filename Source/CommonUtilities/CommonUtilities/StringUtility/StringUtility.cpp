#include "StringUtility.h"

namespace CommonUtilities
{
	void ReplacePartsInString(std::string& aSource, const std::string& aReplace, const std::string& aReplaceWith)
	{
		size_t index = 0;
		while ((index = aSource.find(aReplace, index)) != std::string::npos)
		{
			aSource.replace(index, aReplace.length(), aReplaceWith);
			index += aReplaceWith.length();
		}
	}
}