#include "Weapon.h"
#include <cmath>
#include <assert.h>
#include <typeinfo>

Weapon::Weapon(const Rect* owner , int damage0 , float timer)
	:pOwner(owner) , use_timer(timer), damage(damage0)
{}

void Weapon::CoolDowns(float time)
{
	use_timer.Update(time);
}

void Weapon::Update(float time)
{
	CoolDowns(time);
}

int Weapon::GetDamage() const {
	return damage + dmg_rng.GetVal();
}

RangedWp::RangedWp(const Rect* owner, int damage ,  float speed0, int MaxRounds0, float rTime , float sTime )
	:proj_speed(speed0),
	MaxRounds(MaxRounds0),
	reload(rTime),
	Weapon(owner , damage , sTime)
{
	Rounds = 0;
	BulletCnt = 0;
}

bool RangedWp::CanShoot()const
{
	return (reload.IsReady() && Rounds > 0 && use_timer.IsReady());
}
bool RangedWp::Collision(Rect& rect)
{
	bool collision = false;
	for (auto& proj : proj_list)
	{
		if (proj->GethBox().Collision(rect))
		{
			proj->Destroy();
			collision = true;
		}
	}
	return collision;
}
void RangedWp::Use(Vec2D& target_pos)
{
	if(CanShoot())
	{
		assert(Rounds > 0);
		Vec2D start_pos = { pOwner->left , pOwner->bottom };
		Vec2D vel = { target_pos.x - start_pos.x , target_pos.y - start_pos.y };

		float dist = sqrt(vel.x * vel.x + vel.y * vel.y);
		vel /= dist;
		vel *= proj_speed;

		LaunchNewProj(vel, start_pos);

		use_timer.ResetTimer();
		Rounds--;
		if (Rounds == 0)
		{
			reload.ResetTimer();
		}
	}
}
void RangedWp::Update_projectiles(float time)
{
	proj_list.remove_if([](std::unique_ptr<Projectile>& proj) {
		return proj->IsDestroyed();
		});

	for (auto& proj : proj_list)
	{
		proj->Travel(time);
	}
}
void RangedWp::CoolDowns(float time)
{
	bool BeforeUpdate = reload.IsReady();
	reload.Update(time);
	if (BeforeUpdate == false && reload.IsReady() == true)
	{
		Rounds = 6;
	}
	Weapon::CoolDowns(time);
}

void RangedWp::DrawProjectiles(Graphics& gfx , Camera& ct , Color c)
{
	for (auto& proj : proj_list)
	{
		proj->DrawProjectile(gfx, ct , c);
	}
}
void RangedWp::Draw(Graphics& gfx , Camera& ct)
{
	DrawProjectiles(gfx, ct ,  Colors::Red);
}
int RangedWp::GetCapacity() const {
	return MaxRounds;
}
int RangedWp::GetDamage() const
{
	return damage / 2 + dmg_rng.GetVal();
}

const Rect* Weapon::GetpOwner() const {
	return pOwner;
}

void RangedWp::Update(float time)
{
	CoolDowns(time);
	Update_projectiles(time);
}

void ExplosiveLauncher::LaunchNewProj(Vec2D& vel, Vec2D& initpos)
{
	std::unique_ptr<Explosive> proj = std::make_unique<Explosive>(*InitProj);
	proj->Launch(vel, initpos);
	
	proj_list.emplace_front(std::move(proj));
}
int ExplosiveLauncher::GetDamage() const
{
	return RangedWp::GetDamage() + InitProj->GetDamage();
}
std::forward_list<Rect> RangedWp::GetWeaponHitBoxes() const
{
	std::forward_list<Rect> HitBox_list;
	for (auto& proj : proj_list)
	{
		HitBox_list.emplace_front(proj->GethBox());
	}
	return HitBox_list;
}

