#ifndef TIMEMAN_H
#define TIMEMAN_h

#include <chrono>
#include <thread>

class TimeManager
{
public:
	static TimeManager& Instance()
	{
		static TimeManager instance;

		return instance;
	}
	
	double CalculateFrameRate(bool writeToConsole);
	double TimeManager::GetTime();
	void Sleep(int milliseconds);
	double DeltaTime = 0;
	double CurrentTime = 0;

private:
	TimeManager(){};
	TimeManager(TimeManager const&);
	TimeManager& operator=(TimeManager const&);
};

#endif