#pragma once
#include "Mouse.h"
#include "Timer.h"
#include "Projectile.h"
#include <forward_list>
#include <typeinfo>
#include <memory>


class Weapon {
public:
	Weapon(const Rect* owner, int damage , float usetimer);

	virtual void Draw(Graphics& gfx , Camera& ct) {};
	virtual void CoolDowns(float time);
	virtual void Use(Vec2D& pos) {};
	virtual void Update(float time);
	virtual bool Collision(Rect& rect) { return false; };

	const Rect* GetpOwner() const;
	//this function will get the hitbox of the weapon + it's custom made projectiles
	virtual std::forward_list<Rect> GetWeaponHitBoxes() const { return std::forward_list<Rect> {}; };
	virtual int GetDamage() const;
protected:
	int damage = 0;
	const Rect* pOwner;  // the player , probably
	Timer use_timer;
	Rect weapon;
	mutable RNG dmg_rng = { 0 , 6};
};

//class MeleeWp : public Weapon {
//public:
//	D
//protected:
//	int Reach;
//};
class RangedWp : public Weapon {
public:
	RangedWp() = default;
	RangedWp( const Rect* owner , int damage ,float speed0, int MaxRounds0, float rTime, float sTime);

	//overrides
	std::forward_list<Rect> GetWeaponHitBoxes() const override;
	void Draw(Graphics& gfx , Camera& ct) override;
	void Use(Vec2D& pos)                              override;
	bool Collision(Rect& rect)						  override;
	void Update(float time)							  override;
	void CoolDowns(float time)						  override;
	void Update_projectiles(float time);

	void DrawProjectiles(Graphics& gfx, Camera& ct , Color c);
	// getters
	int GetCapacity() const;
	virtual int GetDamage() const override;

	virtual void LaunchNewProj(Vec2D& vel, Vec2D& initpos) {};

	bool CanShoot() const;
	
protected:
	std::forward_list<std::unique_ptr<Projectile>> proj_list;

	int MaxRounds;
	int Rounds;
	int BulletCnt;

	Timer reload;

	float proj_speed;
protected:
};

class ExplosiveLauncher : public RangedWp {
public:
	ExplosiveLauncher(const Rect* Owner0 ,std::unique_ptr<Explosive> proj, ParticleEffect& particle)
		:InitProj(std::move(proj)),effect_list(particle), RangedWp(Owner0 , 10  ,5.0f , 10 , 1.0f , 0.5f) 
	{
	}
	void LaunchNewProj(Vec2D& vel , Vec2D& initpos) override;
	int GetDamage() const override;
private:
	std::unique_ptr<Explosive> InitProj;
	ParticleEffect& effect_list;
};