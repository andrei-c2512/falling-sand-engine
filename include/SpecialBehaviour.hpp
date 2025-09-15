#pragma once
#include "Element.h"
#include "World.h"

extern RNG Chance;

namespace SpecialBehaviour
{
	class Rd_Cond {
	public:
		Rd_Cond(Type type0 , int chance)
			:type(type0),
			win_chance(chance)
		{
		}
		bool LotteryWon()
		{
			return Chance.GetVal() <= win_chance;
		}
		bool operator()(World& world , int& ind1, int& ind2, Element& elem2)
		{
			if (elem2.GetType() == type)
			{
				return !LotteryWon();
			}
			return false; //aka you passed the test
		}
	private:
		Type type;
		int win_chance;
	};

	class DoNothing
	{
	public:
		DoNothing() = default;
		bool operator()(World& world ,int& ind1, int& ind2, Element& elem2)
		{
			return false;
		}
	private:
	};


	class Sand {
	public:
		bool operator()(World& world , int& ind1, int& ind2, const Element& elem2)
		{
			if (elem2.GetType() == Type::Water)
			{
				if (Chance.GetVal() <= Element::SandSinkChance)
				{
					return true;
				}
			}
			return false;
		}
		static constexpr short spread  = 1;
		static constexpr short gravity = 3;
	};
	class Snow {
	public:
		bool operator()(World& world ,int& ind1, int& ind2, const Element& elem2)
		{
			if (elem2.GetType() == Type::Water)
			{
				if (Chance.GetVal() <= Element::SnowSinkChance)
				{ 
					if (Chance.GetVal() <= Element::SnowToWaterChance)
					{
						world.AddToSpawnList(World::Spawn(ind1, Type::Water));
						return false;
					}
					else
					{
						return false;
					}
				}
				else 
					return true;
			}
			else if (elem2.GetType() == Type::Fire)
			{
				world.AddToSpawnList(World::Spawn(ind1, Type::Water));
				return false;
			}
			return false;
		}
		static constexpr short spread  = 1;
		static constexpr short gravity = 1;
	};

	class Water {
	public:
		bool operator()(World& world , int& ind1, int& ind2, const Element& elem2)
		{
			if (elem2.GetState() == State::Plasma)
			{
				world.AddToConversionList(World::Move(ind2, int(ind1), World::MoveType::Conversion, Type::Steam));
				return true;
			}
			return false;
		}
		static constexpr short spread =  10;
		static constexpr short gravity = 3;
	};

	class Acid {
	public:
		bool operator()(World& world , int& ind1, int& ind2, const Element& elem2)
		{
			State state = elem2.GetState();
			Type type = elem2.GetType(); 
			if ((state != State::Empty && state != State::Plasma) && type != Type::Acid)
			{
				world.AddToConversionList(World::Move(ind1, int(ind2), World::MoveType::Conversion, Type::Water));
				return true;
			}
			return false;
		}
		static constexpr short spread = 10;
		static constexpr short gravity = 3;
	};

	class Smoke {
	public:
		bool operator()(World& world, int& ind1, int& ind2, const Element& elem2)
		{
			return false;
		}
		static constexpr short spread = 1;
		static constexpr short gravity = -1;
	};
	class Steam {
	public:
		bool operator()(World& world, int& ind1, int& ind2, const Element& elem2)
		{
			return false;
		}
		static constexpr short spread = 1;
		static constexpr short gravity = -1;
	};
	class ToxicGas {
	public:
		bool operator()(World& world, int& ind1, int& ind2, const Element& elem2)
		{
			return false;
		}
		static constexpr short spread = 1;
		static constexpr short gravity = -1;
	};
}