#pragma once
#include <chrono>

class Timer
{
public:
	Timer();
	Timer(const Timer& aTimer) = delete;
	Timer(Timer&& aTimer) = delete;
	Timer& operator=(const Timer& aTimer) = delete;
	Timer& operator=(Timer&& aTimer) = delete;

	void	Update();
	
	/// Resets the total time elapsed.
	void	Reset();

	float	GetDeltaTime() const;
	double	GetTotalTime() const;

private:
	std::chrono::high_resolution_clock	myClock;

	std::chrono::high_resolution_clock::time_point myTimeStamp;
	std::chrono::high_resolution_clock::time_point myStart;
	
	float	myDeltaTimeInSeconds;
	double	myTotalTimeInSeconds;
};
