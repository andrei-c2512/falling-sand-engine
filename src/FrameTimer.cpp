#include "FrameTimer.hpp"
#include <string>

using namespace std::chrono;
FrameTimer::FrameTimer()
{
	if (initialized == false)
	{
		last = steady_clock::now();
		initialized = true;
	}
}
float FrameTimer::DeltaTime() {
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frametime = last - old;
	return frametime.count();
}

void FrameShower::DrawFrameCounter(Graphics& gfx , Camera& ct , float time)
{
	timer.Update(time);
	FrameCnt += 1;
	if (timer.IsReady())
	{
		PreviousCnt = FrameCnt;
		FrameCnt = 0;
		timer.ResetTimer();
	}
	int Frames = float(0.1f / (time));

	const std::string frame_str = std::to_string(PreviousCnt);

	int space = 10;
	auto dim = font.GetLetterDim();
	Vec2I pos = Vec2I( Graphics::ScreenWidth - space - frame_str.length() * dim.width ,
				  Graphics::ScreenHeight - (space + dim.height) );

	font.DrawWords(frame_str, gfx, ct , pos);
}