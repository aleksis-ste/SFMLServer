#include "Timer.h"

void Timer::start() {
	startTime = clock();
}

unsigned long Timer::elapsedTime() {
	return ((unsigned long)clock() - startTime) / CLOCKS_PER_SEC;
}

bool Timer::isTimeout(unsigned long seconds) {
	return seconds >= elapsedTime();
}

void Timer::reset() {
	startTime = clock();
}