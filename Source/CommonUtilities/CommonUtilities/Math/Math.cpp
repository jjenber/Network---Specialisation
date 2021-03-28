#include "Math.hpp"
#include <cassert>
namespace CommonUtilities
{
	// Combines two values into one unique. Warning: for optimization reasons this returns a integer and will overflow if pairing too high numbers.
	// If the function should not make a distinction between (a, b) and (b, a) then sort a and b first.
	int SzudzikPairingFunction(int aValue0, int aValue1)
	{
		// http://szudzik.com/ElegantPairing.pdf
		// This is a more space efficent version of Cantors pairing function and also accounts for negative numbers
		int a = aValue0 >= 0 ? 2 * aValue0 : -2 * aValue0 - 1;
		int b = aValue1 >= 0 ? 2 * aValue1 : -2 * aValue1 - 1;
		int value = a >= b ? a * a + a + b : a + b * b;
		assert(value >= 0 && "Pairing value was negative. Most likely due to overflow from big numbers. Consider using long instead of int.");
		return value;
	}

	// Calculates the linear parameter t that produces the interpolant [aValue] within the range[aFrom, aTo].
	float InverseLerp(const float aFrom, const float aTo, const float aValue)
	{
		return (aValue - aFrom) / (aTo - aFrom);
	}
}
