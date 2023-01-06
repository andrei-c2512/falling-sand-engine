#pragma once
#include "Graphics.h"
#include "Timer.h"
#include <forward_list>
#include "Effects.h"
#include "Camera.h"
class Particle {
public:
	Particle(RectI& rect, Color& c, Vec2D& vel0, Timer& lifespan)
		:HitBox(rect) , color(c) , vel(vel0) , LifeSpan(lifespan)
	{
		assert(Graphics::WithinScreen(HitBox));
		Active = true;
	}
	void Draw(Graphics& gfx , Camera& ct)
	{
		Effects::Copy e;
		Vec2I pos0 = ct.Transform(HitBox.GetPos());
		gfx.DrawRect(RectI(HitBox.GetDimensions() , std::move(pos0)), color, e);
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
		return Graphics::WithinScreen(HitBox);
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
		RectI rect = RectI(HitBox.width, HitBox.height, Vec2I(HitBox.left + int(AddX), HitBox.top + int(AddY)));

		if (Graphics::WithinScreen(rect))
		{
			HitBox.left = std::move(rect.left);
			HitBox.top = std::move(rect.top);
		}
		else
			Active = false;
	}
private:
	RectI HitBox;
	Color color;

	Vec2D vel;

	bool Active;
	Timer LifeSpan;
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
	void Draw(Graphics& gfx , Camera& cam)
	{
		for (Particle& particle : particle_list)
		{
			particle.Draw(gfx , cam);
		}
	}
	void AddParticle(Particle& particle)
	{
		particle_list.emplace_front(particle);
	}
private:
	std::forward_list<Particle> particle_list;
};