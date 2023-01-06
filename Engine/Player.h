#pragma once
#include "World.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Entity.h"
#include "NPC.h"
#include <forward_list>
#include "Camera.h"

class Player : public Entity{
public:
	Player(Mouse& mouse ,Sprite& bodysprite  , Sprite& headsprite, Camera& camera ,  float speed);
	void GiveWeapon(std::unique_ptr<Weapon> wp);
	void UpdateMovement(World& world ,Keyboard& kbd , float time);
	void Draw(Graphics& gfx , Camera& ct) override; 
	bool CheckCollision(Entity& entity) override;
	bool CheckCollision(NPC& entity);
	void UseWeapon(float time);
	const Rect* pHitBox() const
	{
		return &HitBox;
	}
public:
	//getters
	const std::forward_list<Rect> GetProj_list();
private:
	Mouse& mouse;
	Sprite sBody;
	Sprite sHead;
	Camera& camera;
	float Speed = 5.0f;

	float Gravity = 15.0f;
	float Friction = 0.5f;
	float JumpPower = 10.0f;
};