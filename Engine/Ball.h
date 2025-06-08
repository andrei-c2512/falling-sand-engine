#pragma once
#include "Figure.h"
#include "CoordinateShower.h"
#include "Button.h"
#include "Font.h"

class Ball {
public:
	Ball(Vec2D pos , int radius);
	void DrawBall(Graphics& gfx ,Camera& camera);
	void ShowClosestDistance(Graphics& gfx, Camera& camera );
	void LaunchBall(Camera& camera, Mouse& mouse);

	void UpdateMovement(FigureField& field , Camera& cam , float time);
	bool CheckCollision(FigureField& field  ,float time);
	bool BorderCollision(float time);
	bool CheckForRebound(Vec2I p1, Vec2I p2 , float time);
	bool ChooseLineForRebound(Vec2I p1, Vec2I p2, Vec2I p3, Vec2I obj, float time);
	void UpdateDist();
public:
	Vec2D GetPos() const
	{
		return pos;
	}
private:
	CameraButton LockToBall;
	CameraButton FreeCamera;
private:
	bool Launched = false;
	Vec2D pos;
	int Radius;
	Vec2D velocity;
	Font font = { "Fixedsys16x28.bmp" };

	float BaseSpeed = 10.0f;
	std::pair<Vec2I, Vec2I> Closest_segment;
	float ClosestDist;
};