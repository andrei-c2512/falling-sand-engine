#pragma once
#include "Mouse.h"
#include "Rect.h"
#include "Timer.h"
#include "Projectile.h"
class Weapon {
public:
	Weapon() = default;
	~Weapon() {
		delete[] pProj;
		pProj = nullptr;
	}
	Weapon(const Weapon& weapon)
		:MaxRounds(weapon.MaxRounds), Rounds(weapon.Rounds),
		BulletCnt(weapon.BulletCnt),
		speed(weapon.speed),reload(weapon.reload),
		shoot(weapon.shoot), pOwner(weapon.pOwner)
	{
		if (weapon.GetpProj() != nullptr)
		{
			pProj = new Projectile[weapon.MaxRounds];

			for (int i = 0; i < weapon.MaxRounds; i++)
			{
				pProj[i] = weapon.pProj[i];
			}
		}
		else
			pProj = nullptr;

	}
	Weapon& operator=(const Weapon& weapon)
	{
		delete[] pProj;
		pProj = nullptr;

		if(weapon.GetpProj() != nullptr)
		{
			pProj = new Projectile[weapon.MaxRounds];

			for (int i = 0; i < weapon.MaxRounds; i++)
			{
				pProj[i] = weapon.pProj[i];
			}
		}

		MaxRounds = weapon.MaxRounds; Rounds = weapon.Rounds;
		BulletCnt = weapon.BulletCnt; 
		shoot = weapon.shoot; reload = weapon.reload;
		speed = weapon.speed;
		pOwner = weapon.pOwner;
		return *this;
	}
	Weapon(Rect* Owner , float speed0 , int MaxRounds0 , float rTime , float sTime);
	void DrawProjectiles(Graphics& gfx , Color c);
	void Shoot(const Mouse& mouse);
	void CoolDowns(float time);
	void BulLifeSpan(float time);
	Rect* GetpOwner() const;
public:
	//getters
	int GetCapacity() const;
	bool IsInitialized() const;
public:
	Projectile* GetpProj() const;
private:
	Rect* pOwner;  // the player , probably
	Projectile* pProj = nullptr;

	int MaxRounds;
	int Rounds;
	int BulletCnt;

	Timer reload;
	Timer shoot;

	bool Initialized = false;
	float speed;
};