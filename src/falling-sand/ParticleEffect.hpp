#pragma once
#include "engine2d/Timer.hpp"
#include "engine2d/math/Rect.hpp"
#include "engine2d/core/Graphics.hpp"
#include "chili/Colors.hpp"
#include <forward_list>

namespace sand {
	class Particle {
	public:
		Particle(const e2d::RectI& rect, chili::Color c, const e2d::Vec2D& vel0, const e2d::Timer& lifespan)
			:HitBox(rect), color(c), vel(vel0), LifeSpan(lifespan)
		{
			assert(e2d::Renderer::WithinScreen(HitBox));
			Active = true;
		}
		void Draw(e2d::Renderer& gfx, const e2d::Camera& ct)
		{
			e2d::Vec2I pos = ct.Transform(HitBox.GetPos());
			gfx.DrawRect(pos.x, pos.y, HitBox.width, HitBox.height, color);
		}
		void Update(float time)
		{
			LifeSpan.Update(time);
			if (LifeSpan.IsReady())
			{
				Active = false;
			}
			else
			{
				UpdateMovement(time);
			}
		}
		bool InBounds()
		{
			//return Graphics::WithinScreen(HitBox);
			return true;
		}
		bool IsActive() const
		{
			return Active;
		}
	private:
		void UpdateMovement(float time)
		{

			float AddX = 60.0f * time * vel.x;
			float AddY = 60.0f * time * vel.y;
			e2d::RectI rect = e2d::RectI(HitBox.width, HitBox.height, e2d::Vec2I(HitBox.left + int(AddX), HitBox.bottom + int(AddY)));

			if (e2d::Renderer::WithinScreen(rect))
			{
				HitBox.left = std::move(rect.left);
				HitBox.bottom = std::move(rect.bottom);
			}
			else
				Active = false;
		}
	private:
		e2d::RectI HitBox;
		chili::Color color;

		e2d::Vec2D vel;

		bool Active;
		e2d::Timer LifeSpan;
	};

	class ParticleEffect {
	public:
		void Update(float time)
		{

			particle_list.remove_if([](Particle& particle)
				{
					return (particle.IsActive() == false || particle.InBounds() == false);
				});

			for (Particle& particle : particle_list)
			{
				particle.Update(time);
			}
		}
		void Draw(e2d::Renderer& gfx, const e2d::Camera& cam)
		{
			for (Particle& particle : particle_list)
			{
				particle.Draw(gfx, cam);
			}
		}
		void AddParticle(const Particle& particle)
		{
			particle_list.emplace_front(particle);
		}
	private:
		std::forward_list<Particle> particle_list;
	};
}