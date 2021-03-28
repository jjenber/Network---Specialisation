#include "Timer.h"

namespace CommonUtilities
{
	Timer::Timer() : myDeltaTimeInSeconds(0), myTotalTimeInSeconds(0)
	{
		Reset();
	}

	void Timer::Update()
	{
		const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(myClock.now() - myTimeStamp);
		const auto total   = std::chrono::duration_cast<std::chrono::microseconds>(myClock.now() - myStart);

		myDeltaTimeInSeconds = static_cast<float>(elapsed.count()) * 0.000001f;
		myTotalTimeInSeconds = static_cast<double>(total.count()) * 0.000001;
		
		myTimeStamp = myClock.now();
	}

	void Timer::Reset()
	{
		myStart = myClock.now();
		myTimeStamp = myStart;
	}

	float Timer::GetDeltaTime() const
	{
		return myDeltaTimeInSeconds;
	}

	double Timer::GetTotalTime() const
	{
		return myTotalTimeInSeconds;
	}

}

