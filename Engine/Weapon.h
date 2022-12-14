#pragma once
#include "Mouse.h"
#include "Timer.h"
#include "Projectile.h"
#include <forward_list>
#include <typeinfo>
#include <memory>

class Weapon {
public:
	Weapon() = default;
	Weapon( float speed0, int MaxRounds0, float rTime, float sTime);
	void Shoot(const Mouse& mouse);
	void Update_projectiles(float time);
	void CoolDowns(float time);
	void DrawProjectiles(Graphics& gfx, Color c);

	// getters
	int GetCapacity() const;
	const std::forward_list<std::unique_ptr<Projectile>>& GetProj_list() const;

	bool IsInitialized() const;
	const Rect* Weapon::GetpOwner() const;

	virtual void LaunchNewProj(Vec2D& vel, Vec2D& initpos) = 0;

	void Update(const Mouse& mouse, float time);
	
protected:
	const Rect* pOwner;  // the player , probably
	std::forward_list<std::unique_ptr<Projectile>> proj_list;

	int MaxRounds;
	int Rounds;
	int BulletCnt;

	Timer reload;
	Timer shoot;

	bool Initialized = false;
	float speed;
protected:
};

class ExplosiveLauncher : public Weapon {
public:
	ExplosiveLauncher(const Rect* Owner0 ,std::unique_ptr<Explosive> proj, ParticleEffect& particle)
		:InitProj(std::move(proj)),effect_list(particle), Weapon(5.0f , 10 , 1.0f , 0.5f) 
	{
		pOwner = Owner0;
	}
	void LaunchNewProj(Vec2D& vel , Vec2D& initpos) override;
private:
	std::unique_ptr<Explosive> InitProj;
	ParticleEffect& effect_list;
};