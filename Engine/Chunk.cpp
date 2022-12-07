#include "Chunk.h"
#include <assert.h>
#include "SpecialBehaviour.h"

bool LotteryWon(int Chance)
{
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> Range(1, 100);

	return Range(rng) <= Chance;
}

void Chunk::Evaluate_Moves(float time ,Order order)
{
	if (Active)
	{
		auto sandbox = world.GetWorld();

		bool IsActive = false;

		int StartX, EndX, AddX;
		if (order == Order::Ascending)
		{
			StartX = Size.left;
			EndX = Size.right();
			AddX = 1;
		}
		else
		{
			StartX = Size.right();
			EndX = Size.left;
			AddX = -1;
		}
		auto dim = world.GetSandboxDim();
		std::vector<World::Move> result;
		for (int y = Size.top ; y < Size.bottom() ; y++)
		{
			for (int x = StartX; x != EndX; x += AddX)
			{
				auto index = y * dim.width + x;
				switch (sandbox[index].GetType())
				{
				default:
					break;
				case Type::Water:
					GetNextMove_Liquid(index, SpecialBehaviour::Water{});
	
					break;
				case Type::Sand:
					GetNextMove_MoveableSolid(index, SpecialBehaviour::Sand{});

					break;
				case Type::Fire:
					GetNextMove_Fire(index);
					world.GetElem(index)-> Update_Fire(time);

					IsActive = true;

					if (Chance.GetVal() <= 30)
					{
						result = EmitFire_Aura(index);

						if (result.size() > 0)
						{
							for (auto b = result.begin(), e = result.end(); e != b; b++)
							{
								world.AddToFireAuraList(*b);
							}
						}
					}
					result.clear();
					break;
				case Type::FireAura:
					world.GetElem(index)->Update_Fire(time);

					break;
				case Type::Smoke:
					GetNextMove_Gas(index , SpecialBehaviour::Smoke{});

					Update_Gas(index, time);
					break;
				case Type::Steam:
					GetNextMove_Gas(index , SpecialBehaviour::Steam{});
		
					world.GetElem(index)->Update_Steam(time);
					break;
				case Type::Snow:
					GetNextRandomMove(index, Direction::Down, SpecialBehaviour::Snow{});

					break;
				case Type::Acid:
					GetNextMove_Liquid(index, SpecialBehaviour::Acid{});
					
					if(Chance.GetVal() > 50)
						Update_Acid(index);
					break;
				case Type::ToxicGas:
					GetNextMove_Gas(index , SpecialBehaviour::ToxicGas{});

					Update_Gas(index, time);

					IsActive = true;
					break;
				}
			}


		}
		Active = IsActive;
	
	}
}



std::pair<bool , Chunk::NextMove> Chunk::IsElemAtBorder(int index)
{
	//assert(Active);
	if (Active == false)
		Active = true;


	auto dim = world.GetSandboxDim();

	NextMove Dir(Direction::None, Direction::None);
	Vec2_<int> pos = Vec2I( index % dim.width , index / dim.width );

	int Spread = 5;

	int right = Size.right();
	int bottom = Size.bottom();

	if (pos.x >= Size.left && pos.x <= Size.left + (Spread))
	{
		Dir.dir1 = Direction::Left;
	}
	else if (pos.x <= right && pos.x >= right - (Spread))
	{
		Dir.dir1 = Direction::Right;
	}

	if (pos.y >= Size.top && pos.y <= Size.top + (Spread))
	{
		Dir.dir2 = Direction::Up;
	}
	else if (pos.y <= bottom && pos.y >= bottom - (Spread))
	{
		Dir.dir2 = Direction::Down;
	}

	bool CanMove = !(Dir == NextMove(Direction::None));

	std::pair<bool, Chunk::NextMove> Action(CanMove, Dir);

	return Action;
}
void Chunk::DrawBorder(Graphics& gfx)
{
	if(Active)
		gfx.DrawRect_Border(PhysicalSize, Colors::Yellow);
}

bool Chunk::InBounds(size_t index, World& world) const
{
	auto dim = world.GetSandboxDim();
	const Vec2_<int> pos = Vec2I( int(index / dim.width) , int(index % dim.width ));

	return pos.x >= Size.left && pos.x <= Size.left + Size.width &&
		pos.y >= Size.top && pos.y <= Size.top + Size.height;
}


int Chunk::GetNextElem(size_t index, Direction dir1, Direction dir2) const
{
	auto dim = world.GetSandboxDim();
	const int Add1 = GetDelta(index, dir1 , dim);
	const int Add2 = GetDelta(index, dir2 , dim);

	const int NewInd = index + Add1 + Add2;

	return NewInd;
}

int Chunk::GetNextElem(size_t index, Direction dir1) const {

	return GetNextElem(index, dir1, Direction::None);

}



int Chunk::GetDelta(int index, Direction dir , Dimensions<size_t> dim) {
	int Delta = 0;
	const int width = int(dim.width);
	const int height = int(dim.height);

	switch (dir)
	{
	case Direction::Left:
		if (index % width != 0)
		{
			Delta = -1;
		}
		break;
	case Direction::Right:
		if (index % width != width - 1)
		{
			Delta = 1;
		}
		break;
	case Direction::Down:
		if (index + width < int(dim.GetArea()))
		{
			Delta = width;
		}
		break;
	case Direction::Up:
		if (index - width >= 0)
		{
			Delta = -width;
		}
		break;
	default:
		Delta = 0;
		break;
	}

	return Delta;
}



void Chunk::SetState(bool state)
{
	Active = state;
}

void Chunk::Necessary_Activation()
{
	if (ActivateNextFrame == true)
	{
		Active = true;
		ActivateNextFrame = false;
	}
}

std::pair<bool , int> Chunk::SpreadFire(size_t index)
{
	auto dim = world.GetSandboxDim();

	std::vector<size_t> Pos_list;

	for (int y = -1; y <= 1; y++)
	{
		size_t AddY = y * dim.width;
		for (int x = -1; x <= 1; x++)
		{
			size_t newind = index + AddY + x;
			
			if (newind >= 0 && newind < dim.width * dim.height)
			{
				if (world.GetElem(newind)->CaughtOnFire())
				{
					Pos_list.emplace_back(newind);
				}
			}
		}
	}

	if (Pos_list.size() != 0)
	{
		RandSpread.ChangeRange(0, int(Pos_list.size()) - 1);

		size_t WorldIndex = Pos_list[RandSpread.GetVal()];

		std::pair<bool, int> result = { true , WorldIndex};

		return result;
	}
	else
	{
		std::pair<bool, int> result(false, -1);
		return result;
	}
}

void Chunk::GetNextMove_Fire(size_t index)
{
	auto result = SpreadFire(index);

	if (result.first == true)
	{
		World::Move move(std::move(result.second) , -1, World::MoveType::SetOnFire);
		world.AddToFireList(std::move(move));
	}

}



void Chunk::Update_Gas(size_t index, float time)
{
	world.GetElem(index)->UpdateLifeSpan(time);
}


std::vector<World::Move> Chunk::EmitFire_Aura(size_t index)
{
	auto dim = world.GetSandboxDim();

	std::vector<size_t> Pos_list;

	for (int y = -1; y <= 0; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			size_t newind = index + y * dim.width + x;

			if (newind >= 0 && newind < dim.GetArea())
				if (world.GetElem(newind)->IsEmpty())
				{
					Pos_list.emplace_back(newind);
				}
		}
	}

	if (Pos_list.size() != 0)
	{
		RandSpread.ChangeRange(0, Pos_list.size() - 1);

		size_t WorldIndex = Pos_list[RandSpread.GetVal()];

		std::vector<World::Move> Fire_list = { { WorldIndex , -1 , World::MoveType::FireAura } };

		size_t FireSize = RandFire.GetVal();

		for (size_t i = 1; i < FireSize; ++i)
		{

			const int NextIndex = int(WorldIndex - dim.width);

			if (NextIndex >= 0)
			{
				if (world.GetElem(NextIndex)->IsEmpty())
				{
					WorldIndex = NextIndex;
					Fire_list.emplace_back( NextIndex , -1 , World::MoveType::FireAura );
				}
			}
			else
				break;
		}

		std::vector<World::Move> result = Fire_list;
		return Fire_list;
	}
	else
	{
		std::vector<World::Move> result;
		return result;
	}
}

void Chunk::Update_Acid(size_t index)
{
	auto dim = world.GetSandboxDim();

	std::vector<size_t> Pos_list;

	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			size_t newind = index + y * dim.width + x;

			if (newind >= 0 && newind < dim.GetArea())
			{
				auto& new_elem = *world.GetElem(newind);
				Type new_type = new_elem.GetType();
				State new_state = new_elem.GetState();

				if (new_state != State::Empty && new_type != Type::Acid && new_type != Type::ToxicGas)
				{
					if(Chance.GetVal() <= Element::AcidDestroyChance)
						Pos_list.emplace_back(newind);
				}
			}
		}
	}
	if (Pos_list.size() != 0)
	{
		RandSpread.ChangeRange(0, Pos_list.size() - 1);

		size_t WorldIndex = Pos_list[RandSpread.GetVal()];
		Type type;
		if (Chance.GetVal() > 60)
			type = Type::Empty;
		else
			type = Type::ToxicGas;

		world.AddToConversionList(World::Move(WorldIndex , World::MoveType::Create , type));
		Active = true;
	}
}