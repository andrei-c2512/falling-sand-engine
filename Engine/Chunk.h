#pragma once
#include "Graphics.h"
#include <vector>
#include "Dimensions.h"
#include "World.h"
#include "SpecialBehaviour.h"
#include "Element.h"
#include <functional>
#include <assert.h>

extern RNG Chance;

class Chunk {
public:
	enum class Direction {
		Left,
		Right,
		Down,
		Up,
		None
	};
	enum class Order {
		Ascending,
		Descending
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
	bool InBounds(size_t index, World& world) const;
	void DrawBorder(Graphics& gfx);

	static int  GetDelta(int index, Direction direction , Dimensions<size_t> dim);               // gets the number of indexes to pass to go to a certain direction on the 2D grid

	int  GetNextElem(size_t index, Direction dir1) const;				     // returns the index of the element in that direction
	int  GetNextElem(size_t index, Direction dir1, Direction dir2) const;    // 

	std::pair<bool , int> SpreadFire(size_t index);

	template<typename E>
	void GetNextMove(size_t index, Direction dir1 , Direction dir2 ,E effect)
	{
		size_t CurInd = index;

		World::MoveType movetype = World::MoveType::Swap;

		Element& elem1 = *world.GetElem(index);
		unsigned char vel;

		if(short(dir1) >= 0 && short(dir1) <= 1 && dir2 == Direction::None)
			vel = effect.spread;
		else
			vel = std::abs(effect.gravity);

		for (unsigned char origin = 0; origin < vel; origin++)
		{
			size_t NextIndex;

			{
				if (dir2 == Direction::None && origin % 6 == 5 &&
					unsigned char(dir1) >= 0 && unsigned char(dir1) <= 1)
				{
					Direction yDir;
					if (effect.gravity > 0)
					{
						yDir = Direction::Down;
					}
					else
						yDir = Direction::Up;

					int new_index = GetNextElem(CurInd, dir1, yDir);
					Element& elem = *world.GetElem(new_index);
					if (elem1.CanMove(elem))
					{
						break;
					}
				}

				NextIndex = GetNextElem(CurInd, dir1, dir2);
			}

			Element& elem2 = *world.GetElem(NextIndex);

			if (elem1.CanMove(elem2))
			{
				auto next_move = effect(world ,index, NextIndex, elem2);
				//basically this is a function that verrifies the special encounters of an element
				//ex: snow turns to water when meeting fire
				if (effect(world, index, NextIndex, elem2))
				{
					elem1.Update();
					goto END;
				}
				if (elem1.GetState() == State::Solid &&
					elem2.GetState() == State::Plasma)
				{
					world.AddToSpawnList(World::Spawn( NextIndex  , Type::Empty ));
				}
				CurInd = NextIndex;
			}
			else
			{
				if (origin == 0)
				{
					goto END;
				}
				break;
			}
		}
		
		world.AddMoveToList(World::Swap(index , CurInd));
		elem1.Update();

	END:
		{
		}
		//end of the program lol
	}

	template<typename E> 
	void  GetNextMove(size_t index, Direction dir1, E effect)
	{
		GetNextMove(index, dir1, Direction::None,effect);
	}
	template<typename E>
	void  GetNextMove(size_t index, NextMove& nextmove, E effect )
	{
		GetNextMove(index, nextmove.dir1, nextmove.dir2, effect);
	}
	template<typename E>
	void GetNextSideMove(size_t index, Direction dirY , E effect )
	{
		bool Option = Rand.GetVal();

		GetNextMove(index, Direction(Option) , dirY, effect);
		auto elem = world.GetElem(index);

		if (elem->IsUpdated() == false)
		{
			GetNextMove(std::move(index), Direction(~Option) , dirY, effect);
		}
	}
	template<typename E>
	void  GetNextRandomMove(size_t index, Direction dirY , E effect)
	{
		int Option = RdMove.GetVal();

		switch (Option)
		{
		case 1:
			GetNextMove(std::move(index), NextMove(Direction::Left, dirY), std::move(effect));
			break;
		case 2:
			GetNextMove(std::move(index), NextMove(Direction::Right, dirY), std::move(effect));
			break;
		case 3:
			GetNextMove(std::move(index), std::move(dirY), std::move(effect));
			break;
		}
	}
	template <typename E>
	void GetNextMove_Liquid(size_t index, E effect)
	{
		assert(world.GetWorld()[index].GetState() == State::Liquid);

		auto elem = world.GetElem(index);
		GetNextMove(index, Direction::Down, effect);

		if (elem->IsUpdated() == false)
		{
			GetNextSideMove(index, Direction::Down, effect);
			if (elem->IsUpdated() == false)
			{
			    GetNextSideMove(index, Direction::None, std::move(effect));
			}
		}
	}
	template <typename E>
	void GetNextMove_MoveableSolid(size_t index, E effect )
	{
		assert(world.GetWorld()[index].GetState() == State::Solid);

		GetNextMove(index, Direction::Down, effect);

		auto elem = world.GetElem(index);
		if (elem->IsUpdated() == false)
		{
			GetNextSideMove(index, Direction::Down, std::move(effect));
		}
	}
	template <typename E>
	void GetNextMove_Gas(size_t index , E effect)
	{
		assert(world.GetWorld()[index].GetState() == State::Gas);

		auto elem = world.GetElem(index);

		GetNextMove(index, Direction::Up, effect);

		if (elem->IsUpdated() == false)
		{
			GetNextSideMove(index, Direction::Up, effect);
			if (elem->IsUpdated() == false)
			{
				GetNextSideMove(index, Direction::None, effect);
			}
		}
	}
	void GetNextMove_Fire(size_t index);
	
	void Evaluate_Moves(float time , Order order);

	void SetState(bool active);
	void Necessary_Activation();
	std::pair<bool, Chunk::NextMove> IsElemAtBorder(int index);
	//-------------------------------
private:
	RNG Rand;
	RNG RandSpread;
	RNG RandFire;
	RNG RdMove;
public:
	std::vector<int> Chunk_matrix;

	RectI Size; // how many elements fit
	RectI PhysicalSize;
	World& world;//the world it takes part of
	bool Active;

	bool ActivateNextFrame;
};