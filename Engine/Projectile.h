#pragma once
#include "Graphics.h"
#include "Vec2D.h"
#include "Rect.h"
#include "World.h"
#include "ParticleEffect.h"
#include "Explosion.h"
class Projectile {
public:
	Projectile() = delete;
	Projectile(Rect& rect,  World& world0 ,Vec2D& vel0, bool Destroyed0);
	Projectile(Rect& rect , World& world0 , float Speed0);
	void InitProj(const Projectile& proj);
	void InitProj(Rect& rect, Vec2D& vel0, bool Destroyed0);
	virtual void Destroy();
	void Launch(Vec2D& vel0, Vec2D& pos0);
	virtual void Travel(float time);
	void DrawProjectile(Graphics& gfx , Color c);
protected:
	void MoveX(float time);
	void MoveY(float time);
	void DetectCollision(float time);
public:
	//getters
	bool IsDestroyed() const;
	Rect GethBox() const;
	Vec2D GetVel() const;
protected:
	Rect HitBox;
	Vec2D vel;
	bool Destroyed;

	float Speed;
protected:
	World& world;
};


class Explosive : public Projectile {
public:
	Explosive(Projectile& proj , ParticleEffect& effect )
		:Projectile(proj) , effect_list(effect) , explosion(world , 15)
	{
		Explode = false;
	}
	void Destroy() override;
	void Travel(float time) override;
private:
	ParticleEffect& effect_list;
	Explosion explosion;

	bool Explode = false;
};