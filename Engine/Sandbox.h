#pragma once
#include "Chunk.h"
#include "World.h"
#include "Weather.h"
#include "MobList.h"
#include "Camera.h"
class Sandbox {
public:
	static constexpr int ChunkSize = 40;
	Sandbox(Mouse& mouse , ParticleEffect& effect , Camera& camera);
	void UpdateSandbox(Mouse& mouse , Keyboard& kbd , float time );
	void DrawSandbox(Graphics& gfx , Camera& , Mouse& mouse);
	void ActivateChunk(int index);
	// for testing
	void DrawChunkBorders(Graphics& gfx , Camera& ct);
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
	Dimensions<int> SandboxDim_InChunks;
	Timer UpdateTimer;
	std::vector<Chunk> Chunk_list;

	RNG rand = { 0 , 1 };

	MobList mob_list;
};