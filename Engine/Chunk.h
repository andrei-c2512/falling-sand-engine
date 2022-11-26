#pragma once
#include "Graphics.h"
#include <vector>
#include "Dimensions.h"
#include "World.h"
#include "SpecialBehaviour.h"
#include "Element.h"
#include <functional>
#include <assert.h>

class Chunk {
public:
	enum class Direction {
		Left,
		Right,
		Down,
		Up,
		None
	};
public:
	struct NextMove {
		NextMove(Direction newdir1)
		{
			dir1 = newdir1;
			dir2 = Direction::None;
		}
		NextMove(Direction newdir1, Direction newdir2)
		{
			dir1 = newdir1;
			dir2 = newdir2;
		}
		bool operator==(NextMove rhs)
		{
			return (dir1 == rhs.dir1 && dir2 == rhs.dir2);
		}
		bool operator!=(NextMove rhs)
		{
			return (dir1 != rhs.dir1 && dir2 != rhs.dir2);
		}
		Direction dir1 = Direction::None;
		Direction dir2 = Direction::None;
	};
public:
	Chunk(RectI size0, World& world0 , size_t index)
		:Size(size0),
		world(world0),
		Rand(0 , 1),
		RandSpread(0 , 1),
		RandFire(1,4),
		Chance(1, 100),
		RdMove(1,3)
	{
		Active = false;
		ActivateNextFrame = false;
		PhysicalSize = Size * World::ElemSize;

		auto dim = world.GetSandboxDim();
		for (int y = Size.top; y < Size.top + Size.height; y++)
		{
			for (int x = Size.left; x < Size.left + Size.width; x++)
			{
				const size_t ElemIndex = y * dim.width + x;
				world.GetElem(ElemIndex)->AssignChunk(index);
				Chunk_matrix.emplace_back(ElemIndex);
			}
		}

	}
	std::vector<World::Move> EmitFire_Aura(size_t index);
	void Update_Gas(size_t index, float time);
	void Update_Acid(size_t index);
	bool InBounds(int index, World& world) const;
	void DrawBorder(Graphics& gfx);

	static int  GetDelta(const int index, Direction direction , Dimensions<size_t> dim);               // gets the number of indexes to pass to go to a certain direction on the 2D grid

	int  GetNextElem(const int index, Direction dir1) const;				     // returns the index of the element in that direction
	int  GetNextElem(const int index, Direction dir1, Direction dir2) const;    // 

	void Move(const int index1, const int index2);
	std::pair<bool , int> SpreadFire(size_t index);

	template<typename E>
	World::Move GetNextMove(const int index, Direction dir1 , Direction dir2 ,E effect = SpecialBehaviour::DoNothing{})
	{
		size_t CurInd = index;

		World::MoveType movetype = World::MoveType::Swap;

		Element& elem1 = *world.GetElem(index);
		int vel;

		if(int(dir1) >= 0 && int(dir1) <= 1 && dir2 == Direction::None)
			vel = elem1.GetSpread();
		else
			vel = std::abs(elem1.GetGravity());

		for (int origin = 0; origin < vel; origin++)
		{
			size_t NextIndex;

			{
				if (dir2 == Direction::None && origin % 3 == 2 &&
					int(dir1) >= 0 && int(dir1) <= 1)
				{
					Direction yDir;
					if (elem1.GetGravity() > 0)
					{
						yDir = Direction::Down;
					}
					else
						yDir = Direction::Up;

					NextIndex = GetNextElem(CurInd, dir1, yDir);
					Element& elem = *world.GetElem(NextIndex);
					if (!elem1.CanMove(elem))
					{
						NextIndex = GetNextElem(CurInd, dir1, dir2);
					}
				}
				else
					NextIndex = GetNextElem(CurInd, dir1, dir2);
			}

			Element& elem2 = *world.GetElem(NextIndex);

			if (elem1.CanMove(elem2))
			{
				auto next_move = effect(CurInd, NextIndex, elem2);
				if (next_move.move != World::MoveType::Swap)
				{
					return std::move(next_move);
				}
				if (world.GetElem(CurInd)->GetState() == State::Solid &&
					elem2.GetState() == State::Plasma)
				{
					world.AddToSpawnList(World::Move{ NextIndex , World::MoveType::Create , Type::Empty });
				}
				CurInd = NextIndex;
			}
			else
			{
				if(origin == 0)
					movetype = World::MoveType::Static;

				break;
			}
		}
		
		World::Move move = World::Move(std::move(index) , std::move(CurInd) , std::move(movetype));

		return move;
	}

	template<typename E> 
	World::Move  GetNextMove(const int index, Direction dir1, E effect = SpecialBehaviour::DoNothing{})
	{
		return GetNextMove(index, dir1, Direction::None,effect);
	}
	template<typename E>
	World::Move  GetNextMove(const int index, NextMove& nextmove, E effect = SpecialBehaviour::DoNothing{})
	{
		return GetNextMove(index, nextmove.dir1, nextmove.dir2, effect);
	}
	template<typename E>
	World::Move  GetNextSideMove(int index, Direction dirY , E effect = SpecialBehaviour::DoNothing{})
	{

		World::Move move (World::MoveType::Static);

		NextMove Move1 = { Direction::Left , dirY};
		NextMove Move2 = { Direction::Right , dirY};

		int Option = Rand.GetVal();
		if (Option == 1)
		{
			std::swap(Move1, Move2);
		}

		move = GetNextMove(index, std::move(Move1), effect);

		if (move.move == World::MoveType::Static)
		{
			return GetNextMove(std::move(index), std::move(Move2), effect);
		}
		else
			return move;
	}
	template<typename E>
	World::Move  GetNextRandomMove(int index, Direction dirY , E effect = SpecialBehaviour::DoNothing{})
	{
		World::Move move(World::MoveType::Static);

		NextMove Move1 = { Direction::Left , dirY };
		NextMove Move2 = { Direction::Right , dirY };


		int Option = RdMove.GetVal();

		switch (Option)
		{
		case 1:
			move = GetNextMove(std::move(index), std::move(Move1), std::move(effect));
			break;
		case 2:
			move = GetNextMove(std::move(index), std::move(Move2), std::move(effect));
			break;
		case 3:
			move = GetNextMove(std::move(index), std::move(dirY), std::move(effect));
			break;
		}

		return move;
	}
	template <typename E>
	World::Move  GetNextMove_Liquid(size_t index, E effect = SpecialBehaviour::DoNothing)
	{
		assert(world.GetWorld()[index].GetState() ==State::Liquid);

		World::Move move = GetNextMove(index, Direction::Down, effect);

		if (move.move == World::MoveType::Static)
		{
			move = GetNextSideMove(index, Direction::Down, effect);
			if (move.move == World::MoveType::Static)
			{
				return GetNextSideMove(index, Direction::None, std::move(effect));
			}
		}
		return move;
	}
	template <typename E>
	World::Move	 GetNextMove_MoveableSolid(size_t index, E effect = SpecialBehaviour::DoNothing)
	{
		assert(world.GetWorld()[index].GetType() == Type::Sand);

		World::Move move = GetNextMove(index, Direction::Down, effect);

		if (move.move == World::MoveType::Static)
		{
			return GetNextSideMove(index, Direction::Down, std::move(effect));
		}
		return move;
	}
	World::Move  GetNextMove_Fire(size_t index);
	World::Move  GetNextMove_Gas(size_t index);
	void Evaluate_Moves(float time);

	void SetState(bool active);
	void Necessary_Activation();
	std::pair<bool, Chunk::NextMove> IsElemAtBorder(int index);
	//-------------------------------
private:
	RNG Rand;
	RNG RandSpread;
	RNG RandFire;
	RNG Chance;
	RNG RdMove;
public:
	std::vector<size_t> Chunk_matrix;

	RectI Size; // how many elements fit
	RectI PhysicalSize;
	World& world;//the world it takes part of
	bool Active;

	bool ActivateNextFrame;
};