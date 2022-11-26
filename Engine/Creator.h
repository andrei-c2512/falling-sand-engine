#pragma once
#include "Sandbox.h"
#include "Mouse.h"
#include "Graphics.h"
#include "Font.h"
#include "Rng.h"
#include "Button.h"
#include "Weather.h"

struct Explosion_verification {
	Explosion_verification(size_t index0 , Action type0)
		:type(type0) , index(index0)
	{
	}
	size_t index;
	Action type;
};
class Creator {
public:
	Creator(RectI& ButtonSize, int Radius , World& world , Weather& weather);
	void Spawn(Mouse& mouse , MouseLastFrameStats& previous_stats, Sandbox& sandbox , ParticleEffect& list);
	void UseTools(Mouse& mouse , Sandbox& sandbox);
	void DrawButtons(Graphics& gfx);
	void DrawSpawnSurface(Graphics& gfx , Mouse& mouse);
	bool CheckButtons(Mouse& mouse);
	bool IsHoveringAButton(Mouse& mouse) const;
	void ExplodeZone(Vec2I& pos ,ParticleEffect& list);
	
	std::vector<size_t> GetSpawnableElements(Mouse& mouse);

	void ShowHoveredElement(Mouse& mouse, Graphics& gfx);
private:
	int SpawnRadius;
	int ExplosionRadius = 30;
	int DarkeningRadius = ExplosionRadius / 3;
	Type ToBeSpawned = Type::Sand;
	Vec2I SpawnLoc = { 0 , 0 };

	World& world;
	Weather& weather;
	Font font = { "Fixedsys16x28.bmp" };

	RNG Chance;
	Vec2I ElemButtonPos = { 10 , 10 };
	Vec2I WeatherButtonPos = { 10 , 30 };

	std::vector<WeatherButton> WButtons;
	std::vector<ElementButton> EButtons;


};