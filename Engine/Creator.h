#pragma once
#include "Sandbox.h"
#include "Mouse.h"
#include "Graphics.h"
#include "Font.h"
#include "Rng.h"
#include "Button.h"
#include "Weather.h"
#include "Explosion.h"
#include <map>
#include "Camera.h"
#include "CoordinateShower.h"
class Creator {
public:
	enum class Action {
		SpawnElement,
		SpawnObject,
		ChangeWeather,
		None,
		Count
	};
public:
	Creator(RectI& ButtonSize, int Radius , Simulation& world , Weather& weather);
	void Spawn(Mouse& mouse , MouseLastFrameStats& previous_stats, Sandbox& sandbox , ParticleEffect& list , Camera& camera);
	void ChangeSpawnArea(Mouse& mouse);
	void DrawButtons(Graphics& gfx , Camera& ct);
	void DrawSpawnSurface(Graphics& gfx , Camera& ct , Mouse& mouse);
	bool CheckButtons(Mouse& mouse);
	bool IsHoveringAButton(Mouse& mouse) const;
	std::vector<size_t> GetSpawnableElements(Mouse& mouse , Camera& camera);

	void ShowHoveredElement(Mouse& mouse, Graphics& gfx , Camera& ct);
	
private:
	float SpawnRadius;
	float ExplosionRadius = 30;
	float DarkeningRadius = ExplosionRadius / 3;

	float MinSpawnRadius = 1;
	float MinExplosionRadius = 6;

	int Max_rows = 3; // for buttons
	int space = 10; // space between buttons
	Type ToBeSpawned = Type::Sand;
	Vec2I SpawnLoc = { 0 , 0 };

	World& world;
	Weather& weather;
	Font font = { "Fixedsys16x28.bmp" };

	RNG Chance;
	Vec2I ElemButtonPos = { 10 , 590 };
	Vec2I WeatherButtonPos = { 10 , 30 + ((int(Type::Count) / (Max_rows)) + 1) * (ElementButton::dim + space)};

	int InfoY = 30;

	std::vector<WeatherButton> WButtons;
	std::vector<ElementButton> EButtons;
	std::map<Type, std::string> ElemName_map;
	std::map<WeatherType, std::string> WeatherName_map;

	Explosion explosion;
};