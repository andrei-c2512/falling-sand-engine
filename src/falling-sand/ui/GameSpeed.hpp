#pragma once
#include "chili/Mouse.hpp"

#include "engine2d/core/Graphics.hpp"
#include "engine2d/ui/Button.hpp"


namespace sand {
	class GameSpeed {
	public:
		GameSpeed() = default;
		GameSpeed(int dim0, e2d::Vec2D pos0, float level0, float Cld0);
		void DrawButton(e2d::Renderer& gfx);
		void ChangeValue(const chili::Mouse& mouse);
		void CounterLife(float dt);
		float GetLevel();
	private:
		int Dim = 20;
		e2d::Vec2D pos;
		float level;
		float Cld;
		float Counter;
	};
}