#include <string>
namespace CommonUtilities
{
	// Replaces all occurances of [aReplace] in [aSource] with [aReplaceWith].
	void ReplacePartsInString(std::string& aSource, const std::string& aReplace, const std::string& aReplaceWith);
}
