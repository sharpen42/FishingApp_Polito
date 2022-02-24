#pragma once

enum class timer_state {
	timer_inactive, timer_counting, timer_stopped, timer_ended
};

class Timer {
private: 
	uint16_t id;
	timer_state status;
	float count;
	void (*atEnd)(int);

public:
	double match;

	Timer();
	Timer(double match);
	bool start();
	bool start(float c);
	bool start(void(*atEnd)(int));
	bool pass(float time);
	bool stop();
	bool go();
	bool isCounting();
	unsigned int getId();
};