#include "Weapon.h"
#include <cmath>
#include <assert.h>

Weapon::Weapon(Rect* Owner0 , float speed0, int MaxRounds0, float rTime , float sTime )
	:speed(speed0),
	MaxRounds(MaxRounds0),
	reload(rTime),
	shoot(sTime)
{
	Initialized = false;
	Rounds = 0;
	BulletCnt = 0;

	pProj = new Projectile[MaxRounds];
	for (int i = 0; i < MaxRounds; i++)
		pProj[i].InitProj(Rect(5, 5, Vec2D(NULL, NULL)) , Vec2D( 0.0f , 0.0f ) , true);

	pOwner = Owner0;
	Initialized = true;
}

void Weapon::Shoot(const Mouse& mouse)
{
	if (mouse.LeftIsPressed() && reload.IsReady() && Rounds > 0)
	{
		int x = mouse.GetPosX() , y = mouse.GetPosY();
		if (shoot.IsReady())
		{
			assert(Rounds > 0);
			Vec2D pos = { pOwner->left , pOwner->top };
			Vec2D vel = { x - pos.x , y - pos.y };
			
			float dist = sqrt(vel.x * vel.x + vel.y * vel.y);
			vel /= dist;
			vel *= speed;

			pProj[BulletCnt].Launch(vel , pos);

			BulletCnt++;
			if (BulletCnt == MaxRounds)
				BulletCnt = 0;

			shoot.ResetTimer();
			Rounds--;
			if (Rounds == 0)
			{
				reload.ResetTimer();
			}
		}
	}
}
void Weapon::BulLifeSpan(float time)
{
	for (int i = 0; i < MaxRounds; i++)
		pProj[i].Travel(time);
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
	for (int i = 0; i < MaxRounds; i++)
		pProj[i].DrawProjectile(gfx,  c );
}

int Weapon::GetCapacity() const {
	return MaxRounds;
}

Projectile* Weapon::GetpProj() const {
	return pProj;
}

bool Weapon::IsInitialized() const {
	return Initialized;
}

Rect* Weapon::GetpOwner() const {
	return pOwner;
}