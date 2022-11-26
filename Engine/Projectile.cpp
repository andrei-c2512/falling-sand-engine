#include "Projectile.h"


Projectile::Projectile(Rect& rect, Vec2D& vel0, bool Destroyed0)
	:hBox(rect),
	vel(vel0),
	Destroyed(Destroyed0)
{
}

void Projectile::InitProj(Rect& rect, Vec2D& vel0, bool Destroyed0)
{
	hBox = rect; vel = vel0; Destroyed = Destroyed0;
}
void Projectile::InitProj(const Projectile& proj)
{
	hBox = proj.GethBox();
	Destroyed = proj.IsDestroyed();
	vel = proj.GetVel();
}
void Projectile::Launch(Vec2D& vel0, Vec2D& pos0)
{
	Destroyed = false;
	vel = vel0;
	hBox.left = pos0.x;
	hBox.top = pos0.y;
}

void Projectile::Destroy()
{
	Destroyed = true;
	vel = { 0.0f , 0.0f };
	hBox.left = hBox.top = NULL;
}

void Projectile::Travel(float time)
{
	if (!Destroyed)
	{
		float* x = &hBox.left;
		float* y = &hBox.top;

		float addX = 60.0f * vel.x * time, addY = 60.0f * vel.y * time;

		if ((*x + addX + hBox.width) < Graphics::ScreenWidth && int(*x + addX) >= 0 &&
			(*y + addY + hBox.height) < Graphics::ScreenHeight && int(*y + addY) >= 0)
		{
			*x += addX;
			*y += addY;
		}
		else
			Destroy();
	}
}
void Projectile::DrawProjectile(Graphics& gfx , Color c) {
	if (Destroyed == false)
		gfx.DrawRect0(hBox, c);
}

Rect Projectile::GethBox() const
{
	return hBox;
}


bool Projectile::IsDestroyed() const
{
	return Destroyed;
}

Vec2D Projectile::GetVel() const
{
	return vel;
}