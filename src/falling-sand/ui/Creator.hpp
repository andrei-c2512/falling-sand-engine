#pragma once
#include "chili/Mouse.hpp"
#include "engine2d/core/Graphics.hpp"
#include "engine2d/render/Font.hpp"
#include "engine2d/ui/CoordinateShower.hpp"

#include "falling-sand/Sandbox.hpp"
#include "falling-sand/ui/ElementButton.hpp"
#include "falling-sand/ui/WeatherButton.hpp"
#include <map>

namespace sand {
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
		Creator(const e2d::RectI& ButtonSize, int Radius, Simulation& world, Weather& weather);
		void Spawn(const chili::Mouse& mouse, const chili::MouseLastFrameStats& previous_stats, 
			Sandbox& sandbox, ParticleEffect& list, const e2d::Camera& camera);
		void ChangeSpawnArea(chili::Mouse& mouse);
		void DrawButtons(e2d::Renderer& gfx, const e2d::Camera& ct);
		void DrawSpawnSurface(e2d::Renderer& gfx, const e2d::Camera& ct, const chili::Mouse& mouse);
		bool CheckButtons(const chili::Mouse& mouse);
		bool IsHoveringAButton(const chili::Mouse& mouse) const;
		std::vector<size_t> GetSpawnableElements(const chili::Mouse& mouse, const e2d::Camera& camera);

		void ShowHoveredElement(const chili::Mouse& mouse, e2d::Renderer& gfx, const e2d::Camera& ct);

	private:
		float SpawnRadius;
		float ExplosionRadius = 30;
		float DarkeningRadius = ExplosionRadius / 3;

		float MinSpawnRadius = 1;
		float MinExplosionRadius = 6;

		int Max_rows = 3; // for buttons
		int space = 10; // space between buttons
		Type ToBeSpawned = Type::Sand;
		e2d::Vec2I SpawnLoc = { 0 , 0 };

		World& world;
		Weather& weather;
		e2d::Font font = { "assets/Fixedsys16x28.bmp" };

		e2d::RNG Chance;
		e2d::Vec2I ElemButtonPos = { 10 , 590 };
		e2d::Vec2I WeatherButtonPos = { 10 , 30 + ((int(Type::Count) / (Max_rows)) + 1) * (ElementButton::dim + space) };

		int InfoY = 30;

		std::vector<WeatherButton> WButtons;
		std::vector<ElementButton> EButtons;
		std::map<Type, std::string> ElemName_map;
		std::map<WeatherType, std::string> WeatherName_map;

		Explosion explosion;
	};
}