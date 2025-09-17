#include "GameSpeed.hpp"

GameSpeed::GameSpeed(int dim0 , Vec2D pos0 , float level0 , float Cld0) 
	:Dim(dim0),
	pos(pos0),
	level(level0)
{
	Counter = 0.0f;
}
void GameSpeed::DrawButton(Graphics& gfx) {
	for (int y = pos.y; y < pos.y + Dim; y++)
	{
		for (int x = pos.x; x < pos.x + Dim; x++)
			gfx.PutPixel(x, y, 255, 255, 255);
	}
}
void GameSpeed::ChangeValue(Mouse& mouse)
{
	int x = mouse.GetPosX(), y = mouse.GetPosY();
	if (x >= pos.x && x <= pos.x + Dim &&
		y >= pos.y && y <= pos.y + Dim && Counter >= Cld)
	{
		if (mouse.LeftIsPressed() && level > 0.25f)
		{
			level -= 0.25f;
			Counter = 0.0f;
		}
		else if (mouse.RightIsPressed() && level < 5.00f)
		{
			level += 0.25f;
			Counter = 0.0f;
		}
	}
	
}
void GameSpeed::CounterLife(float dt)
{
	if (Counter < Cld)
		Counter += dt;
}
float GameSpeed::GetLevel()
{
	return level;
}