#pragma once
#include "Graphics.h"
#include "Vec2D.h"
#include "Mouse.h"
#include "Button.h"

class GameSpeed {
public:
	GameSpeed() = default;
	GameSpeed(int dim0, Vec2D pos0, float level0 , float Cld0);
	void DrawButton(Graphics& gfx);
	void ChangeValue(Mouse& mouse);
	void CounterLife(float dt);
	float GetLevel();
private:
	int Dim = 20;
	Vec2D pos;
	float level;
	float Cld;
	float Counter;
};