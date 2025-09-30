#include "FrameTimer.hpp"
#include <string>

using namespace std::chrono;
namespace e2d {
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

	void FrameShower::DrawFrameCounter(e2d::Renderer& gfx, const Camera& ct, float time)
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
		Vec2I pos = Vec2I(e2d::Renderer::ScreenWidth - space - frame_str.length() * dim.width,
			e2d::Renderer::ScreenHeight - (space + dim.height));

		gfx.DrawString(pos.x, pos.y, frame_str.data(), font);
	}
}