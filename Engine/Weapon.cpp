#include "Weapon.h"
#include <cmath>
#include <assert.h>
#include <typeinfo>

Weapon::Weapon( float speed0, int MaxRounds0, float rTime , float sTime )
	:speed(speed0),
	MaxRounds(MaxRounds0),
	reload(rTime),
	shoot(sTime)
{
	Initialized = false;
	Rounds = 0;
	BulletCnt = 0;
	Initialized = true;
}

void Weapon::Shoot(const Mouse& mouse)
{
	if (mouse.LeftIsPressed() && reload.IsReady() && Rounds > 0)
	{
		short x = mouse.GetPosX() , y = mouse.GetPosY();
		if (shoot.IsReady())
		{
			assert(Rounds > 0);
			Vec2D pos = { pOwner->left , pOwner->top };
			Vec2D vel = { x - pos.x , y - pos.y };
			
			float dist = sqrt(vel.x * vel.x + vel.y * vel.y);
			vel /= dist;
			vel *= speed;

			LaunchNewProj(vel, pos);

			shoot.ResetTimer();
			Rounds--;
			if (Rounds == 0)
			{
				reload.ResetTimer();
			}
		}
	}
}
void Weapon::Update_projectiles(float time)
{
	for (auto& proj : proj_list)
	{
		proj->Travel(time);
	}
}
void Weapon::CoolDowns(float time)
{
	bool BeforeUpdate = reload.IsReady();
	reload.Update(time);
	if (BeforeUpdate == false && reload.IsReady() == true)
	{
		Rounds = 6;
	}
	shoot.Update(time);
}
void Weapon::DrawProjectiles(Graphics& gfx , Color c)
{
	for (auto& proj : proj_list)
	{
		proj->DrawProjectile(gfx, c);
	}
}

int Weapon::GetCapacity() const {
	return MaxRounds;
}


bool Weapon::IsInitialized() const {
	return Initialized;
}

const Rect* Weapon::GetpOwner() const {
	return pOwner;
}

void Weapon::Update(const Mouse& mouse, float time)
{
	CoolDowns(time);
	Update_projectiles(time);
	Shoot(mouse);
}

void ExplosiveLauncher::LaunchNewProj(Vec2D& vel, Vec2D& initpos)
{
	std::unique_ptr<Explosive> proj = std::make_unique<Explosive>(*InitProj);
	proj->Launch(vel, initpos);
	
	proj_list.emplace_front(std::move(proj));
}