#pragma once
#include "World.hpp"
#include "ParticleEffect.hpp"
#include "Explosion.hpp"
#include "engine2d/core/Graphics.hpp"

namespace sand {
	class Projectile {
	public:
		Projectile() = delete;
		Projectile(const e2d::Rect& rect, Simulation& world0, const e2d::Vec2D& vel0, bool Destroyed0);
		Projectile(const e2d::Rect& rect, Simulation& world0, float Speed0);
		void InitProj(const Projectile& proj);
		void InitProj(const e2d::Rect& rect, const e2d::Vec2D& vel0, bool Destroyed0);
		virtual void Destroy();
		void Launch(const e2d::Vec2D& vel0, const e2d::Vec2D& pos0);
		virtual void Travel(float time);
		void DrawProjectile(e2d::Renderer& gfx, const e2d::Camera& ct, chili::Color c);
	protected:
		void MoveX(float time);
		void MoveY(float time);

		void MoveRight(float time);
		void MoveLeft(float time);
		void MoveDown(float time);
		void MoveUp(float time);

		void DetectCollision(float time);
	public:
		//getters
		bool IsDestroyed() const;
		e2d::Rect GethBox() const;
		e2d::Vec2D GetVel() const;
		float GetDamage() const;
	protected:
		e2d::Rect HitBox;
		e2d::Vec2D vel;
		bool Destroyed;

		float Speed;
		float BaseDamage = 5;
	protected:
		Simulation& simulation;
		mutable e2d::RNG dmg_rand = { -3 , 3 };
	};


	class Explosive : public Projectile {
	public:
		Explosive(const Projectile& proj, ParticleEffect& effect)
			:Projectile(proj), effect_list(effect), explosion(simulation)
		{
			BaseDamage = 20;
			Explode = false;
		}
		void Destroy() override;
		void Travel(float time) override;
	private:
		ParticleEffect& effect_list;
		Explosion explosion;

		float ExplosionRadius = 10.0f;
		float DarkeningRadius = ExplosionRadius / 3.0f;
		bool Explode = false;
	};
}