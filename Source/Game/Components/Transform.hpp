#pragma once
#include <CommonUtilities\Math\Vector.h>

namespace components
{
	struct Transform
	{
		cu::Vector3f myPosition;
		cu::Vector3f myRotation;
		cu::Vector3f myScale;
	};
}