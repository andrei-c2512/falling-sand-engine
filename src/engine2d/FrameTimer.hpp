#pragma once
#include <chrono>
#include "engine2d/core/Graphics.hpp"
#include "engine2d/render/Font.hpp"
#include "Timer.hpp"
#include "engine2d/pipeline/Camera.hpp"

namespace e2d {
	class FrameTimer {
	public:
		FrameTimer();
		float DeltaTime();
		void DrawFrameCounter(e2d::Renderer& gfx, const e2d::Camera& ct, float time);
	private:
		std::chrono::steady_clock::time_point last;
		bool initialized = false;
	};

	class FrameShower {
	public:
		FrameShower() = default;
		void DrawFrameCounter(e2d::Renderer& gfx, const Camera& ct, float time);

	private:
		Timer timer = { 1.0f };
		int FrameCnt = 0;
		int PreviousCnt = 0;

		Font font = { "assets/Fixedsys16x28.bmp" };
	};
}