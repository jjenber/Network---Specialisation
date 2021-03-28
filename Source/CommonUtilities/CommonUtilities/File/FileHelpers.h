#pragma once
#include <string>

namespace CommonUtilities
{
	class FileHelpers
	{
	public:
		static bool Exists(const std::string& aFilePath);
	};
}

