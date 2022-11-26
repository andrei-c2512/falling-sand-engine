#pragma once
#include "Graphics.h"
#include "Timer.h"
#include <forward_list>
class Particle {
public:
	Particle(RectI& rect, Color& c, Vec2D& vel0, Timer& lifespan)
		:HitBox(rect) , color(c) , vel(vel0) , LifeSpan(lifespan)
	{
		Active = true;
	}
	void Draw(Graphics& gfx)
	{
		gfx.DrawRectI(HitBox, color);
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
		float Add = 60.0f * time;
		HitBox.left += Add * vel.x;
		HitBox.top  += Add * vel.y;
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
	void Draw(Graphics& gfx)
	{
		for (Particle& particle : particle_list)
		{
			particle.Draw(gfx);
		}
	}
	void AddParticle(Particle& particle)
	{
		particle_list.emplace_front(particle);
	}
private:
	std::forward_list<Particle> particle_list;
};