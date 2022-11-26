#pragma once
#include "Graphics.h"
#include "Vec2D.h"
#include "Rect.h"

class Projectile {
public:
	Projectile() = default;
	Projectile(Rect& rect, Vec2D& vel0, bool Destroyed0);
	void InitProj(const Projectile& proj);
	void InitProj(Rect& rect, Vec2D& vel0, bool Destroyed0);
	void Destroy();
	void Launch(Vec2D& vel0, Vec2D& pos0);
	void Travel(float time);
	void DrawProjectile(Graphics& gfx , Color c);
public:
	//getters
	bool IsDestroyed() const;
	Rect GethBox() const;
	Vec2D GetVel() const;
private:
	Rect hBox;
	Vec2D vel;
	bool Destroyed;
};