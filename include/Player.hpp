#pragma once
#include "World.hpp"
#include "Graphics.hpp"
#include "Keyboard.hpp"
#include "Entity.hpp"
#include "NPC.hpp"
#include <forward_list>
#include "Camera.hpp"

class Player : public Entity{
public:
	Player(Mouse& mouse , const Sprite& bodysprite  , const Sprite& headsprite, Camera& camera ,  float speed);
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