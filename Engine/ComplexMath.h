#pragma once
#define PI 3.14159265f
#include <cmath>
#include <assert.h>
#include "Vec2D.h"
#include "Rect.h"
#include <map>
#include <functional>

class MoveAngleBy {
public:
	static void MoveBy(Vec2D vel, int degrees)
	{
		//x is cos
		assert(vel.x >= -1.0f && vel.x <= 1.0f);
		//y is sin
		assert(vel.y >= -1.0f && vel.y <= 1.0f);

		//basically im creating a 90x90 rectangle that will be split in 4 45x45 rectangles that represents the "cadranele cercului"
		//idk how its called in english 
		//now i will be converting the cos and sin so that hyphotenuse is 45

		float sin = vel.y * 45.0f;
		float cos = vel.x * 45.0f;

		int Cadran = 1;
		if (sin >= 0.0f)
		{
			if (cos >= 0.0f)
			{
				Cadran = 1;
			}
			else
			{
				Cadran = 2;
			}
		}
		else
		{
			if (cos >= 0.0f)
			{
				Cadran = 4;
			}
			else
			{
				Cadran = 3;
			}
		}
	}
private:
	Vec2D GetAngle(Vec2D vel, int cadran)
	{
		Vec2D angle = { 0.0f , 0.0f };
		switch (cadran)
		{
		case 1:
			angle = Cadran1(vel);
			break;
		case 2:
			angle = Cadran2(vel);
			break;
		case 3:
			angle = Cadran3(vel);
			break;
		case 4:
			angle = Cadran4(vel);
			break;
		}
	}
	Vec2D Cadran1(Vec2D vel);
	Vec2D Cadran2(Vec2D vel);
	Vec2D Cadran3(Vec2D vel);
	Vec2D Cadran4(Vec2D vel);
};