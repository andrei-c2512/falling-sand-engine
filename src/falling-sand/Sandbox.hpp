#pragma once
#include "Chunk.hpp"
#include "World.hpp"
#include "Weather.hpp"
#include "engine2d/pipeline/Camera.hpp"
#include "engine2d/core/Graphics.hpp"
#include "falling-sand/MobList.hpp"
#include "chili/Mouse.hpp"
#include "chili/Keyboard.hpp"


namespace sand {
	class Sandbox {
	public:
		static constexpr int ChunkSize = 40;
		Sandbox(chili::Mouse& mouse, ParticleEffect& effect, e2d::Camera& camera);
		void UpdateSandbox(const chili::Mouse& mouse, const chili::Keyboard& kbd, float time);
		void DrawSandbox(e2d::Renderer& gfx, const e2d::Camera&, const chili::Mouse& mouse);
		void ActivateChunk(int index);
		// for testing
		void DrawChunkBorders(e2d::Renderer& gfx, const e2d::Camera& ct);
		void UpdateTime(float time);

		Chunk* GetChunk(size_t elemindex);
		size_t GetChunkIndex(size_t elemindex);
		Chunk* GetAdjacentChunk(size_t chunkindex, Chunk::NextMove dir);

		void ActivateAdjacentChunks(size_t index, Chunk::NextMove dir);
		void UpdateChunkActivation();
		void ActivateNeededChunks(size_t elem);
		// getters

		Simulation& GetWorld() {
			return simulation;
		}
		Weather& GetWeather() {
			return weather;
		}
	private:
		Simulation simulation;
		Weather weather;
		e2d::Size<int> SandboxDim_InChunks;
		e2d::Timer UpdateTimer;
		std::vector<Chunk> Chunk_list;

		MobList mob_list;
		e2d::RNG rand = { 0 , 1 };
	};
}