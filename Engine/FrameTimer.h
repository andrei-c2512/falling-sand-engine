#pragma once
#include <chrono>
#include "Graphics.h"
#include "Font.h"
#include "Timer.h"
class FrameTimer {
public:
	FrameTimer();
	float DeltaTime();
	void DrawFrameCounter(Graphics& gfx, float time);
private:
	std::chrono::steady_clock::time_point last;
	bool initialized = false;
};

class FrameShower {
public:
	FrameShower() = default;
	void DrawFrameCounter(Graphics& gfx, float time);

private:
	Timer timer = { 1.0f };
	int FrameCnt = 0;
	int PreviousCnt = 0;

	Font font = { "Fixedsys16x28.bmp" };
};