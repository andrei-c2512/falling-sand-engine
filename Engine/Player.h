#pragma once
#include "World.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Weapon.h"
#include <forward_list>
class Player{
public:
	Player(Sprite& bodysprite  , Sprite& headsprite, World& world ,  float speed);
	void GiveWeapon(std::unique_ptr<Weapon> wp);
	void UpdateMovement(Keyboard& kbd , float time);
	void Draw(Graphics& gfx , Mouse& mouse);
	void Move(Keyboard& kbd, float time);
	void UseWeapon(Mouse& mouse , float time);
	const Rect* pHitBox() const
	{
		return &HitBox;
	}
public:
	//getters
	const std::forward_list<std::unique_ptr<Projectile>>& GetProj_list() const;
	Rect GetRect() const;
private:
	void MoveX(float time);
	void MoveY(float time);
private:
	std::unique_ptr<Weapon> pWeapon;
	Rect HitBox;

	Sprite sBody;
	Sprite sHead;

	World& world;

	Vec2D vel;
	float Speed = 10.0f;

	float Gravity = 15.0f;
	float Friction = 0.5f;
	float JumpPower = 10.0f;
};