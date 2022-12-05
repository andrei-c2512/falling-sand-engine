#include "Sandbox.h"
#include "assert.h"

Sandbox::Sandbox(ParticleEffect& effect) 
	:weather(world , WeatherType::Clear , size_t(2)), 
	player(Sprite(Dimensions<short>(10, 20), Colors::Yellow), Sprite(Dimensions<short>(10, 10), Colors::Yellow)
		, world, 10.0f)
{
	{
		auto proj = std::make_unique<Explosive>(Explosive(Projectile(Rect(5, 5, Vec2D(5, 5)), world, 5.0f) , effect)) ;

		std::unique_ptr<ExplosiveLauncher> wp = std::make_unique<ExplosiveLauncher>
			(ExplosiveLauncher(player.pHitBox(), std::move(proj) , effect));
		
		player.GiveWeapon(std::move(wp));

		auto dim = world.GetSandboxDim();
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
				assert(index < SandboxDim_InChunks.GetArea());
				Chunk_list.emplace_back(RectI(CWidth, CHeight, Vec2I(x, y)), world, index);

			}
		}

		UpdateTimer = { 0.016f };
		
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

		for (Chunk& chunk : Chunk_list)
		{
			chunk.Evaluate_Moves(time);
		}
		UpdateChunkActivation();
		world.CommitMoves();
		weather.UpdateMatrix();
		
		player.UpdateMovement(kbd , time);
		player.UseWeapon(mouse , time);
		UpdateTimer.ResetTimer();
	}
}

void Sandbox::UpdateTime(float time)
{
	UpdateTimer.Update(time);
}

void Sandbox::DrawSandbox(Graphics& gfx ,Mouse& mouse)
{
	world.DrawWorld(gfx);
	player.Draw(gfx, mouse);
	DrawChunkBorders(gfx);
}

void Sandbox::ActivateChunk(int index)
{
	const size_t ChunkInd = world.GetElem(index)->GetChunkIndex();
	Chunk_list[ChunkInd].Active = true;
}

void Sandbox::DrawChunkBorders(Graphics& gfx)
{
	//for (auto& chunk : Chunk_list)
	//{
	//	chunk.DrawBorder(gfx);
	//}
}

void Sandbox::UpdateChunkActivation()
{
	auto List = world.GetMove_List();

	for (size_t i = 0; i < world.GetMove_ListSize(); i++)
	{
		ActivateNeededChunks(List[i].index1);
	}

	auto List1 = world.GetFire_list();

	for (size_t i = 0 ; i < world.GetFire_listSize() ; i++)
	{
		ActivateChunk(List1[i].elem1);
	}

	auto List2 = world.GetFireAura_list();

	for (auto b = List2.begin() , e = List2.end() ; b != e ; b++)
	{
		ActivateChunk(b->elem1);
	}

	auto List3 = world.GetSpawn_list();

	for (auto b = List3.begin(), e = List3.end(); b != e; b++)
	{
		ActivateChunk(b->elem1);
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
	const auto dim = world.GetSandboxDim();
	Vec2I ElemPos( elemindex % dim.width , elemindex / dim.width );
	Vec2I ChunkPos( ElemPos.x / ChunkSize , ElemPos.y / ChunkSize );

	return ChunkPos.y * SandboxDim_InChunks.width + ChunkPos.x;
}
Chunk* Sandbox::GetChunk(size_t elemindex)
{
	const auto dim = world.GetSandboxDim();
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