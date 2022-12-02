#pragma once
//#include "Projectile.h"
//#include "Graphics.h"
//#include "Timer.h"
//#include <vector>
////mob weapon
//class MobWp {
//public:
//	enum class Behaviour {
//		Automatic, // shoot one after one
//		Burst, // shoots 2 after 2
//		Explosive, // shoot at the player location , only to explode at that point
//		Shotgun, // shoot like a shotgun all the projectiles
//		Invalid
//	};
//public:
//	MobWp() = default;
//	MobWp(Rect* owner, float speed, Projectile& proj , Behaviour behaviour0, float sTime, float rTime);
//	MobWp(float speed, Projectile& proj, Behaviour behaviour0, float sTime, float rTime);
//	// this second constructor is for cases for specific cases
//	// usually we will assign the owner of the weapon in GiveWeapon() function of the entity
//	~MobWp()
//	{
//		pOwner = nullptr;
//
//		pTarget = nullptr;  // we do not destroy what they are pointing as they point to moving objects . 
//		// these are basically a read only pointer
//	}
//	MobWp(const MobWp& wp) 
//		:pOwner(new Rect), pTarget(new Rect),
//		speed(wp.speed) , shoot(wp.shoot),
//		reload(wp.reload),InitProj(wp.InitProj),
//		behaviour(wp.behaviour)
//
//	{
//		pOwner = pTarget = nullptr;
//
//		if (wp.pOwner != nullptr)
//		{
//			pOwner = new Rect;
//			pOwner = wp.pOwner;
//		}
//
//
//		if (wp.pTarget != nullptr)
//		{
//			pTarget = new Rect;
//			pTarget = wp.pTarget;
//		}
//	}
//	MobWp& operator=(const MobWp& wp)
//	{
//		pOwner = pTarget = nullptr;
//
//		if (wp.pOwner != nullptr)
//		{
//			pOwner = new Rect;
//			pOwner = wp.pOwner;
//		}
//
//
//		if (wp.pTarget != nullptr)
//		{
//			pTarget = new Rect;
//			pTarget = wp.pTarget;
//		}
//
//
//		speed = wp.speed; shoot = wp.shoot;
//		reload = wp.reload; InitProj = wp.InitProj;
//		behaviour = wp.behaviour;
//		return *this;
//	}
//	void DrawProjectiles(Graphics& gfx , Color c);   
//	void UpdateTimers(float time);
//	void ShootTarget(Rect& target);
//	void UpdateTarget(Rect* rect);
//	Vec2D Trajectory(); // return the velocity of the future projectile
//	void HitTarget(); // this will veriffy if the target was hit
//
//	//Making sure the projectiles will actually travel
//	void ProjLifeSpan(float time);
//public:
//	//Behaviour
//	Behaviour GetRdBehaviour() const;
//	void AutomaticB();
//	void BurstB();
//	void ExplosiveB();
//	void ShotgunB();
//public:
//	int GetIndOfNextProj();
//	Rect* GetpOwner() const;
//	void SetpOwner(Rect* NewOwner);
//private:
//	Rect* pOwner; // this will point to the owner of the weapon aka the mob's hitbox
//
//	float speed;
//	Timer shoot; 
//	Timer reload;
//
//	std::vector<Projectile> proj = { InitProj };
//
//	Rect* pTarget;
//	Projectile InitProj; // the settings of the initial proj
//
//	Behaviour  behaviour;
//};