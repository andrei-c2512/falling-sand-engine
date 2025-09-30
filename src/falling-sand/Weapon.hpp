#pragma once
#include "chili/Mouse.hpp"
#include "engine2d/Timer.hpp"
#include "Projectile.hpp"
#include <forward_list>
#include <typeinfo>
#include <memory>


namespace sand {
	class Weapon {
	public:
		Weapon(const e2d::Rect* owner, int damage, float usetimer);

		virtual void Draw(e2d::Renderer& gfx, e2d::Camera& ct) {};
		virtual void CoolDowns(float time);
		virtual void Use(const e2d::Vec2D& pos) {};
		virtual void Update(float time);
		virtual bool Collision(const e2d::Rect& rect) { return false; };

		const e2d::Rect* GetpOwner() const;
		//this function will get the hitbox of the weapon + it's custom made projectiles
		virtual std::forward_list<e2d::Rect> GetWeaponHitBoxes() const { return std::forward_list<e2d::Rect> {}; };
		virtual int GetDamage() const;
	protected:
		int damage = 0;
		const e2d::Rect* pOwner;  // the player , probably
		e2d::Timer use_timer;
		e2d::Rect weapon;
		mutable e2d::RNG dmg_rng = { 0 , 6 };
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
		RangedWp(const e2d::Rect* owner, int damage, float speed0, int MaxRounds0, float rTime, float sTime);

		//overrides
		std::forward_list<e2d::Rect> GetWeaponHitBoxes() const override;
		void Draw(e2d::Renderer& gfx, e2d::Camera& ct) override;
		void Use(const e2d::Vec2D& pos)                              override;
		bool Collision(const e2d::Rect& rect)				  override;
		void Update(float time)							  override;
		void CoolDowns(float time)						  override;
		void Update_projectiles(float time);

		void DrawProjectiles(e2d::Renderer& gfx, const e2d::Camera& ct, chili::Color c);
		// getters
		int GetCapacity() const;
		virtual int GetDamage() const override;

		virtual void LaunchNewProj(const e2d::Vec2D& vel, const e2d::Vec2D& initpos) {};

		bool CanShoot() const;

	protected:
		std::forward_list<std::unique_ptr<Projectile>> proj_list;

		int MaxRounds;
		int Rounds;
		int BulletCnt;

		e2d::Timer reload;

		float proj_speed;
	protected:
	};

	class ExplosiveLauncher : public RangedWp {
	public:
		ExplosiveLauncher(const e2d::Rect* Owner0, std::unique_ptr<Explosive> proj, ParticleEffect& particle)
			:InitProj(std::move(proj)), effect_list(particle), RangedWp(Owner0, 10, 5.0f, 10, 1.0f, 0.5f)
		{
		}
		void LaunchNewProj(const e2d::Vec2D& vel, const e2d::Vec2D& initpos) override;
		int GetDamage() const override;
	private:
		std::unique_ptr<Explosive> InitProj;
		ParticleEffect& effect_list;
	};
}