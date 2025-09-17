#pragma once
#include <chrono>
#include "Graphics.hpp"
#include "Font.hpp"
#include "Timer.hpp"
#include "CoordinateTransformer.hpp"
class FrameTimer {
public:
	FrameTimer();
	float DeltaTime();
	void DrawFrameCounter(Graphics& gfx, Camera& ct , float time);
private:
	std::chrono::steady_clock::time_point last;
	bool initialized = false;
};

class FrameShower {
public:
	FrameShower() = default;
	void DrawFrameCounter(Graphics& gfx, Camera& ct ,  float time);

private:
	Timer timer = { 1.0f };
	int FrameCnt = 0;
	int PreviousCnt = 0;

	Font font = { "assets/Fixedsys16x28.bmp" };
};