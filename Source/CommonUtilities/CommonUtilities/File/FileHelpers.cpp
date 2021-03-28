#include "FileHelpers.h"
#include <filesystem>

namespace CommonUtilities
{
    bool FileHelpers::Exists(const std::string& aFilePath)
    {
        return std::filesystem::exists(aFilePath);
    }
}
