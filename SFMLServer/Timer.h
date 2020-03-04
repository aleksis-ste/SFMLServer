#pragma once
#include <ctime>

class Timer 
{
private:
	unsigned long startTime;

public:

	void start();

	unsigned long elapsedTime();

	bool isTimeout(unsigned long seconds);

	void reset();

};
