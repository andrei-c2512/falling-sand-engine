#include "Sandbox.hpp"
#include "assert.h"

Sandbox::Sandbox(Mouse& mouse , ParticleEffect& effect , Camera& camera) 
	:weather(simulation , WeatherType::Clear , size_t(2)),
	mob_list(simulation ,effect , mouse , camera)
{
	{
		auto dim = simulation.GetSandboxDim();
		const int width = dim.width;
		const int height = dim.height;


		int DeltaX = 0, DeltaY = 0;
		bool UseDelta = false;
		if (width % ChunkSize != 0)
		{
			DeltaX = ChunkSize - width % ChunkSize;
			UseDelta = true;
		}

		if (height % ChunkSize != 0)
		{
			DeltaY = ChunkSize - height % ChunkSize;
			UseDelta = true;
		}

		SandboxDim_InChunks.height = height / ChunkSize;
		if (height % ChunkSize != 0)
			SandboxDim_InChunks.height++;

		SandboxDim_InChunks.width = width / ChunkSize;
		if (width % ChunkSize != 0)
			SandboxDim_InChunks.height++;


		for (int y = 0; y < height; y += ChunkSize)
		{
			for (int x = 0; x < width; x += ChunkSize)
			{
				int CWidth, CHeight;
				CWidth = CHeight = ChunkSize;

				if (UseDelta)
				{
					if (y + ChunkSize >= height)
					{
						CHeight -= DeltaY;
					}
					if (x + ChunkSize >= width)
					{
						CWidth -= DeltaX;
					}
				}

				const size_t index = (y / ChunkSize) * SandboxDim_InChunks.width + (x / ChunkSize);
				//assert(index < SandboxDim_InChunks.GetArea());
				Chunk_list.emplace_back(RectI(CWidth, CHeight, Vec2I(x, y)), simulation, index);

			}
		}

		UpdateTimer = { 0.008f };
		
	}
}
void Sandbox::UpdateSandbox(Mouse& mouse , Keyboard& kbd , float time)
{
	if (UpdateTimer.IsReady())
	{
		for (auto& n : Chunk_list)
		{
			n.Necessary_Activation();
		}

		
		{
			Chunk::Order order = Chunk::Order(rand.GetVal());
			for (Chunk& chunk : Chunk_list)
			{
				chunk.Evaluate_Moves(time , Chunk::Order(order));
			}
			UpdateChunkActivation();
			simulation.CommitMoves();
		}

		simulation.Go(time);
		weather.UpdateMatrix();
	

		mob_list.Go(time);
		mob_list.UpdateMobs(kbd , time);

		UpdateTimer.ResetTimer();

	}
}

void Sandbox::UpdateTime(float time)
{
	UpdateTimer.Update(time);
}

void Sandbox::DrawSandbox(Graphics& gfx , Camera& cam , Mouse& mouse)
{
	simulation.DrawWorld(gfx , cam);
	mob_list.DrawMobs(gfx , cam);
	DrawChunkBorders(gfx , cam);
}

void Sandbox::ActivateChunk(int index)
{
	const size_t ChunkInd = simulation.GetElem(index)->GetChunkIndex();
	Chunk_list[ChunkInd].Active = true;
}

void Sandbox::DrawChunkBorders(Graphics& gfx ,Camera& cam )
{
	for (auto& chunk : Chunk_list)
	{
		//chunk.DrawBorder(gfx , cam);
	}
}

void Sandbox::UpdateChunkActivation()
{
	auto List = simulation.GetMove_List();

	for (size_t i = 0; i < simulation.GetMove_ListSize(); i++)
	{
		ActivateNeededChunks(List[i].index1);
	}

	auto List1 = simulation.GetFire_list();

	for (size_t i = 0 ; i < simulation.GetFire_listSize() ; i++)
	{
		ActivateChunk(List1[i].elem1);
	}

	auto List2 = simulation.GetFireAura_list();

	for (auto b = List2.begin() , e = List2.end() ; b != e ; b++)
	{
		ActivateChunk(b->elem1);
	}

	auto List3 = simulation.GetSpawn_list();

	for (auto b = List3.begin(), e = List3.end(); b != e; b++)
	{
		ActivateChunk(b->index);
	}
}

void Sandbox::ActivateAdjacentChunks(size_t index, Chunk::NextMove dir)
{
	const size_t ind = index;

	int delta = 0;

	if (dir.dir1 != Chunk::Direction::None)
	{
		auto chunk = GetAdjacentChunk(ind, dir.dir1);
		chunk->ActivateNextFrame = true;
	}

	if (dir.dir2 != Chunk::Direction::None)
	{
		auto chunk = GetAdjacentChunk(ind, dir.dir2);
		chunk->ActivateNextFrame = true;
	}

	if (dir != Chunk::NextMove(Chunk::Direction::None))
	{
		auto chunk = GetAdjacentChunk(ind, dir);
		chunk->ActivateNextFrame = true;
	}

}

size_t Sandbox::GetChunkIndex(size_t elemindex)
{
	const auto dim = simulation.GetSandboxDim();
	Vec2I ElemPos( elemindex % dim.width , elemindex / dim.width );
	Vec2I ChunkPos( ElemPos.x / ChunkSize , ElemPos.y / ChunkSize );

	return ChunkPos.y * SandboxDim_InChunks.width + ChunkPos.x;
}
Chunk* Sandbox::GetChunk(size_t elemindex)
{
	const auto dim = simulation.GetSandboxDim();
	Vec2I ElemPos ( elemindex % dim.width , elemindex / dim.width );
	Vec2I ChunkPos ( ElemPos.x / ChunkSize , ElemPos.y / ChunkSize );

	return &Chunk_list[ChunkPos.y * SandboxDim_InChunks.width + ChunkPos.x];
}

Chunk* Sandbox::GetAdjacentChunk(size_t chunkindex, Chunk::NextMove dir)
{
	int delta = Chunk::GetDelta(chunkindex, dir.dir1, SandboxDim_InChunks);
	delta += Chunk::GetDelta(chunkindex, dir.dir2, SandboxDim_InChunks);

	return &Chunk_list[chunkindex + delta];
}

void Sandbox::ActivateNeededChunks(size_t elem)
{
	size_t chunkInd0 = GetChunkIndex(elem);
	auto Action = Chunk_list[chunkInd0].IsElemAtBorder(elem);

	if(Action.first == true)
	{
		ActivateAdjacentChunks(chunkInd0, Action.second);
	}
}