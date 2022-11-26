#include "MobWp.h"
#include <random>
#include <assert.h>

MobWp::MobWp(Rect* owner, float speed, Projectile& proj , Behaviour behaviour0 , float sTime , float rTime)
	:pOwner(owner) ,
	speed(speed) ,
	InitProj(proj),
	behaviour(behaviour0),
	reload(rTime), shoot(sTime)
{
	pTarget = nullptr;
}
void MobWp::AutomaticB()
{
	if (shoot.IsReady() && pTarget != nullptr)
	{
		unsigned int ind = GetIndOfNextProj();

		if (ind == proj.size())
		{
			proj.emplace_back(InitProj);  // I first initialize the new projectile
		}
		else
		{
			proj[ind] = InitProj;
		}


		assert(pOwner != nullptr);
		proj[ind].Launch(Trajectory(), Vec2D(pOwner->left , pOwner->top));

		shoot.ResetTimer();
	}
}

void MobWp::UpdateTarget(Rect* target)
{
	pTarget = target;
}

int MobWp::GetIndOfNextProj()
{
	unsigned int ind;
	for ( ind = 0 ; ind < proj.size() ; ind++)
	{
		if (proj[ind].IsDestroyed())
		{
			return ind;
		}
	}
 // i return a size that's +1 bigger than the size of the vector
	return ind;
}

Vec2D MobWp::Trajectory()
{
	Vec2D vel = { 0.0f , 0.0f };
	Vec2D target = { pTarget->left , pTarget->top};
	Vec2D pos = { pOwner->left , pOwner->top };

	float dist = pos.GetLenght(target);

	vel = { target.x - pos.x , target.y - pos.y };
	vel /= dist;
	vel *= speed;

	return vel;
}

void MobWp::HitTarget()
{
	for (unsigned int i = 0; i < proj.size(); i++)
	{
		if(proj[i].IsDestroyed() == false)
			if (pTarget->Collision(proj[i].GethBox()))
			{
				proj[i].Destroy();
			}

	}
}

void MobWp::ProjLifeSpan(float time)
{
	for (unsigned int i = 0; i < proj.size(); i++)
	{
		proj[i].Travel(time);
	}
}

void MobWp::DrawProjectiles(Graphics& gfx, Color c)
{
	for (unsigned int i = 0; i < proj.size(); i++)
	{
		proj[i].DrawProjectile(gfx, c);
	}
}

MobWp::Behaviour MobWp::GetRdBehaviour() const
{
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> pick(1, 4);

	int Pick = pick(rng);

	Behaviour behaviour = Behaviour::Invalid;
	switch (Pick)
	{
	case 1:
		behaviour = Behaviour::Automatic;
		break;
	case 2:
		behaviour = Behaviour::Shotgun;
		break;
	case 3:
		behaviour = Behaviour::Burst;
		break;
	case 4:
		behaviour = Behaviour::Explosive;
		break;
	}

	return behaviour;
}

void MobWp::UpdateTimers(float time)
{
	shoot.Update(time);
	reload.Update(time);
}

Rect* MobWp::GetpOwner() const {
	return pOwner;
}

void MobWp::SetpOwner(Rect* NewOwner){
	pOwner = NewOwner;
}