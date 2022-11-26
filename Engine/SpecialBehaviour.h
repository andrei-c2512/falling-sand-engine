#pragma once
#include "Element.h"
#include "World.h"

namespace SpecialBehaviour
{

	
	class Rd_Cond {
	public:
		Rd_Cond(Type type0 , int chance)
			:type(type0),
			Chance(chance)
		{
		}
		bool LotteryWon()
		{
			return rng.GetVal() <= Chance;
		}
		bool operator()(size_t& ind1, size_t& ind2, Element& elem2)
		{
			if (elem2.GetType() == type)
			{
				if (LotteryWon() == false)
				{
					return true;
				}
				else
					return false;
			}
			return false; //aka you passed the test
		}
	private:
		Type type;
		int Chance;
		RNG rng = { 1, 100 };
	};

	class DoNothing
	{
	public:
		DoNothing() = default;
		World::Move operator()(size_t& ind1, size_t& ind2, Element& elem2)
		{
			return World::Move{ World::MoveType::Swap };
		}
	private:
	};
	class Sand {
	public:
		World::Move operator()(size_t& ind1, size_t& ind2, Element& elem2)
		{
			if (elem2.GetType() == Type::Water)
			{
				if (Chance.GetVal() <= Element::SandSinkChance)
				{
					return World::Move{ World::MoveType::Static };
				}
			}
			return World::Move{ World::MoveType::Swap };
		}
	private:
		RNG Chance = { 1 , 100 };
	};
	class Snow {
	public:
		World::Move operator()(size_t& ind1, size_t& ind2, Element& elem2)
		{
			if (elem2.GetType() == Type::Water)
			{
				if (Chance.GetVal() <= Element::SnowSinkChance)
				{ 
					if (Chance.GetVal() <= Element::SnowToWaterChance)
					{
						return World::Move(ind1,  World::MoveType::Create, Type::Water);
					}
					else
					{
						return World::Move(World::MoveType::Swap);
					}
				}
				else 
					return World::Move(World::MoveType::Static);
			}
			else if (elem2.GetType() == Type::Fire)
			{
				return World::Move(ind1, World::MoveType::Create, Type::Water);
			}
			return World::Move(World::MoveType::Swap);
		}
	private:
		RNG Chance = { 1 , 100 };
	};

	class Water {
	public:
		World::Move operator()(size_t& ind1, size_t& ind2, Element& elem2)
		{
			if (elem2.GetState() == State::Plasma)
			{
				return World::Move(ind1, ind2, World::MoveType::Conversion, Type::Steam);
			}
			return World::Move(World::MoveType::Swap);
		}
	private:
		RNG Chance = { 1 , 100 };
	};

	class Acid {
	public:
		World::Move operator()(size_t& ind1, size_t& ind2, Element& elem2)
		{
			State state = elem2.GetState();
			Type type = elem2.GetType(); 
			if ((state != State::Empty && state != State::Plasma) && type != Type::Acid)
			{
				return World::Move(ind1, ind2, World::MoveType::Conversion, Type::Water);
			}
			return World::Move(World::MoveType::Swap);
		}
	private:
		RNG Chance = { 1 , 100 };
	};
}