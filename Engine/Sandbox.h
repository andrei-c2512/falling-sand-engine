#pragma once
#include "Chunk.h"
#include "World.h"
#include "Weather.h"
#include "Player.h"
class Sandbox {
public:
	static constexpr int ChunkSize = 40;
	Sandbox();
	void UpdateSandbox(Keyboard& kbd , float time );
	void DrawSandbox(Graphics& gfx , Mouse& mouse);
	void ActivateChunk(int index);
	// for testing
	void DrawChunkBorders(Graphics& gfx);
	void UpdateTime(float time);

	Chunk* GetChunk(size_t elemindex);
	size_t GetChunkIndex(size_t elemindex);
	Chunk* GetAdjacentChunk(size_t chunkindex, Chunk::NextMove dir);
	 
	void ActivateAdjacentChunks(size_t index, Chunk::NextMove dir);
	void UpdateChunkActivation();
	void ActivateNeededChunks(size_t elem);
	// getters

	World& GetWorld() {
		return world;
	}
	Weather& GetWeather() {
		return weather;
	}
private:
	World world;
	Weather weather;
	Dimensions<size_t> SandboxDim_InChunks;
	Timer UpdateTimer;
	std::vector<Chunk> Chunk_list;

	Player player;
};