#pragma once
#include "Player.hpp"
#include <map>
#include <memory>
#include "NPC.hpp"
#include "World.hpp"
#include "engine2d/pipeline/Camera.hpp"
#include "chili/Mouse.hpp"
#include "chili/Keyboard.hpp"

namespace sand {
	class MobList {
	public:
		enum class MobType {
			Zombie,
			Walker,
			BigMama
		};
	public:
		MobList(Simulation& world0, ParticleEffect& effect, chili::Mouse& mouse, e2d::Camera& camera)
			:Zombie(Entity(e2d::Rect(25, 25, e2d::Vec2D(0, 0)), e2d::Sprite(e2d::Size<short>(10, 15), chili::Colors::Red), 10, 100, 2.0f)),
			player(mouse, e2d::Sprite(e2d::Size<short>(10, 20), chili::Colors::Yellow), e2d::Sprite(e2d::Size<short>(10, 10), chili::Colors::Yellow),
				camera, 5.0f),
			simulation(world0)
		{
			auto proj = std::make_unique<Explosive>(Explosive(Projectile(e2d::Rect(5, 5, e2d::Vec2D(5, 5)), simulation, 10.0f), effect));

			std::unique_ptr<ExplosiveLauncher> wp = std::make_unique<ExplosiveLauncher>
				(ExplosiveLauncher(player.pHitBox(), std::move(proj), effect));

			player.GiveWeapon(std::move(wp));

			entity_map.emplace(MobType::Zombie, Zombie);

			SpawnLoc = { 400 , 200 };
		}
		void UpdateMobs(const chili::Keyboard& kbd, float time)
		{
			player.UseWeapon(time);
			player.UpdateMovement(simulation, kbd, time);

			for (auto& entity : entity_list)
			{
				entity->DetermineMovement();
				entity->Move(simulation, time);
			}

			for (auto& entity : entity_list)
			{
				player.CheckCollision(*entity);
			}

			entity_list.remove_if([](std::unique_ptr<NPC>& npc)
				{
					return npc->IsDead();
				});
		}
		void DrawMobs(e2d::Renderer& gfx, const e2d::Camera& cam)
		{
			for (auto& entity : entity_list)
			{
				entity->Draw(gfx, cam);
			}
			player.Draw(gfx, cam);
		}
		void SpawnMob(MobType mobtype)
		{
			std::unique_ptr<NPC> mob = std::make_unique<NPC>(entity_map[mobtype]);
			mob->SetPos(SpawnLoc);
			entity_list.emplace_front(std::move(mob));
		}
		void Go(float time)
		{
			Spawn_timer.Update(time);
			if (Spawn_timer.IsReady())
			{
				//SpawnMob(MobType::Zombie);
				//Spawn_timer.ResetTimer();
			}
		}
	private:
		std::forward_list<std::unique_ptr<NPC>> entity_list;
		e2d::Vec2I SpawnLoc;

		NPC Zombie;
		e2d::Timer Spawn_timer = 2.0f;
		std::map<MobType, NPC> entity_map;

		Player player;
		Simulation& simulation;
	};
}