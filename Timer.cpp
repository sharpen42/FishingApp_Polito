#include <random>

#include "Timer.h"

static int timer_n = 0;

void defaultEnd(int n) {
	return;
}

Timer::Timer()
{
	status = timer_state::timer_inactive;
	count = 0.0;
	match = 0.0;
	atEnd = defaultEnd;
	id = ++timer_n;
}

Timer::Timer(double match0){
	status = timer_state::timer_inactive;
	count = 0.0;
	match = match0;
	atEnd = defaultEnd;
	id = ++timer_n;
}

bool Timer::start()
{
	if (status == timer_state::timer_inactive || status == timer_state::timer_ended) {
		status = timer_state::timer_counting;
		count = 0.0;
		return true;
	}
	return false;
}

bool Timer::start(void(*atEnd0)(int))
{
	if (status == timer_state::timer_inactive || status == timer_state::timer_ended) {
		status = timer_state::timer_counting;
		count = 0.0;
		atEnd = atEnd0;
		return true;
	}
	return false;
}

bool Timer::pass(float time)
{
	if (status == timer_state::timer_counting) {
		count += time;
		if (count >= match) {
			status = timer_state::timer_ended;
			atEnd(id);
		}
		return true;
	}
	return false;
}

bool Timer::stop()
{
	if (status == timer_state::timer_counting) {
		status = timer_state::timer_stopped;
		return true;
	}
	return false;
}

bool Timer::go()
{
	if (status == timer_state::timer_stopped) {
		status = timer_state::timer_counting;
		return true;
	}
	return false;
}

unsigned int Timer::getId() { return id; }
