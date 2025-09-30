#pragma once
#include "falling-sand/World.hpp"
#include "falling-sand/Entity.hpp"
#include "falling-sand/NPC.hpp"

#include "engine2d/core/Graphics.hpp"
#include "engine2d/pipeline/Camera.hpp"

#include "chili/Keyboard.hpp"
#include "chili/Mouse.hpp"

#include <forward_list>

namespace sand {
	class Player : public Entity {
	public:
		Player(chili::Mouse& mouse, const e2d::Sprite& bodysprite, const e2d::Sprite& headsprite, e2d::Camera& camera, float speed);
		void GiveWeapon(std::unique_ptr<Weapon> wp);
		void UpdateMovement(World& world, const chili::Keyboard& kbd, float time);
		void Draw(e2d::Renderer& gfx, const e2d::Camera& ct) override;
		bool CheckCollision(Entity& entity) override;
		bool CheckCollision(NPC& entity);
		void UseWeapon(float time);
		const e2d::Rect* pHitBox() const
		{
			return &HitBox;
		}
	public:
		//getters
		const std::forward_list<e2d::Rect> GetProj_list();
	private:
		chili::Mouse& mouse;
		e2d::Sprite sBody;
		e2d::Sprite sHead;
		e2d::Camera& camera;
		float Speed = 5.0f;

		float Gravity = 15.0f;
		float Friction = 0.5f;
		float JumpPower = 10.0f;
	};
}