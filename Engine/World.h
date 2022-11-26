#pragma once
#include "Element.h"
#include "Graphics.h"
#include <vector>
#include <algorithm>
#include "Dimensions.h"
#include "Rect.h"
#include "assert.h"
#include "Effects.h"

class World {
public:
	enum class MoveType {
		Swap,
		Conversion,
		SetOnFire,
		FireAura,
		Static,
		Special,
		Create,
		Count
	};
public:
	struct Move {
		Move() = default;
		Move(size_t elem_1, int elem_2, MoveType movetype)
			:elem1(elem_1), elem2(elem_2) , move(movetype)
		{
		}
		Move(size_t elem_1, int elem_2, MoveType movetype , Type conversion0)
			:elem1(elem_1), elem2(elem_2), move(movetype) , conversion(conversion0)
		{
		}
		Move(size_t elem_1, MoveType movetype, Type conversion0)
			:elem1(elem_1), move(movetype), conversion(conversion0)
		{
		}
		Move(World::MoveType movetype)
			:move(movetype)
		{
		}
		void MergeElem(World& world)
		{
			assert(elem2 >= 0);

			if(world.Chance.GetVal() > 80)
				world.GetElem(elem1)->Create(conversion);

			world.GetElem(elem2)->Create(Type::Empty);
		}
		void MoveElem(World& world)
		{
			assert(elem2 != -1);

			Element& elem_1 = *world.GetElem(elem1);
			Element& elem_2 = *world.GetElem(elem2);

			elem_1.SwapPositions(elem_2);
		}
		void CommitMove(World& world)
		{
			switch (move)
			{
			case MoveType::Swap:
				MoveElem(world);
				break;
			case MoveType::SetOnFire:
				world.GetElem(elem1)->SetOnFire();
				break;
			case MoveType::FireAura:
				world.GetElem(elem1)->Create(Type::FireAura);
				break;
			case MoveType::Conversion:
				assert(conversion != Type::Empty);
				MergeElem(world);
				break;
			case MoveType::Create:
				world.CreateElem(elem1 , conversion);
				break;
			}
		}
		size_t elem1 = 0;
		int elem2 = -1;
		MoveType move = MoveType::Static;
		Type conversion = Type::Empty;
	};
public:
	World()
		:Chance(1 ,100), Pick(0 , 2)
	{
		SandboxDim.width = Graphics::ScreenWidth / ElemSize;
		SandboxDim.height = Graphics::ScreenHeight / ElemSize;

		const int width = SandboxDim.width;
		const int height = SandboxDim.height;

		Elements = std::vector<Element>(width * height, Element(RectI(ElemSize, ElemSize, Vec2I(NULL, NULL))));

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				Vec2D Pos = Vec2D(float(x * ElemSize), float(y * ElemSize));
				Elements[y * width + x].Update(Pos);
			}
		}
		//blur.Blur(BackGround);
		
	}
	void DrawWorld(Graphics& gfx)
	{
		const int width = BackGround.GetWidth();
		const int height = BackGround.GetHeight();

		Effects::Copy e{};
	//	gfx.DrawSprite(0, 0, BackGround, RectI(width, height, Vec2I(0, 0)), Graphics::GetScreenRect(), e);
		for (auto& n : Elements)
		{
			n.DrawElement(gfx , BackGround);
		}
	}

	static constexpr int ElemSize = 4;

	static constexpr int SandSinkChance = 75;
	static constexpr int WoodFlamability = 50;


	Element* GetWorld()
	{
		return Elements.data();
	}
	std::vector<Move> GetMove_List()
	{
		return Move_List;
	}
	size_t GetMove_ListSize() const {
		return Move_List.size();
	}
	Dimensions<size_t> GetSandboxDim() const
	{
		return SandboxDim;
	}

	bool CreateElem(size_t index, Type type)
	{
		if (index >= 0 && index < Elements.size())
		{
			Elements[index].Create(type);
			return true;
		}
		return false;
	}

	void AddMoveToList(Move& move)
	{
		Move_List.emplace_back(move);
	}
	void AddToFireList(Move& move)
	{
		Fire_list.emplace_back(move);
	}
	void AddToFireAuraList(Move& move)
	{
		FireAura_list.emplace_back(move);
	}
	void AddToConversionList(Move& move)
	{
		Conversion_list.emplace_back(move);
	}
	void AddToSpawnList(Move& move) 
	{
		Spawn_list.emplace_back(move);
	}
	size_t GetFire_listSize() const
	{
		return Fire_list.size();
	}
	std::vector<Move> GetSpawn_list()
	{
		return Spawn_list;
	}
	std::vector<Move> GetFire_list()
	{
		return Fire_list;
	}
	std::vector<Move> GetFireAura_list() const {
		return FireAura_list;
	}
	std::vector<Move> GetConversion_list() const {
		return Conversion_list;
	}

	void CommitMoves()
	{
		for (auto& move : Fire_list)
		{
			move.CommitMove(*this);
		}
		for (auto& move : FireAura_list)
		{
			move.CommitMove(*this);
		}
		for (auto& move : Conversion_list)
		{
			move.CommitMove(*this);
		}
		for (auto& move : Spawn_list)
		{
			move.CommitMove(*this);
		}

		for (auto& n : Elements)
		{
			n.ResetStatus();
		}
		std::sort(Move_List.begin(), Move_List.end(), [](Move& move1, Move& move2)
			{
				return move1.elem2 > move2.elem2;
			});
		for (size_t ind = 0; ind < Move_List.size(); )
		{
			size_t add = 1;
			while (ind + add < Move_List.size())
			{
				if (Move_List[ind + add].elem2 == Move_List[ind].elem2)
					add++;
				else
					break;
			}
			if (add == 1)
			{
				Move_List[ind].CommitMove(*this);
			}
			else
			{
				Pick.ChangeRange(ind, ind + add - 1);
				Move_List[Pick.GetVal()].CommitMove(*this);
			}
			ind += add;
		}

		Fire_list.clear();
		Move_List.clear();
		FireAura_list.clear();
		Conversion_list.clear();
		Spawn_list.clear();
	}
	Element* GetElem(size_t index)
	{
		return &Elements[index];
	}
	Vec2I IndexToPos(size_t index)
	{
		return Vec2I(index % SandboxDim.width , index / SandboxDim.width );
	}
	Vec2I ScreenToMatrixPos(Vec2I& pos)
	{
		return Vec2I(pos.x / ElemSize , pos.y / ElemSize);
	}
	size_t MatrixPosToIndex(Vec2I& pos)
	{
		return pos.y * SandboxDim.width + pos.x;
	}
	size_t GetElem(Vec2I& pos)
	{
		return pos.y * SandboxDim.width + pos.x;
	}
	size_t GetElemScr(Vec2I& screenpos)
	{
		return GetElem(ScreenToMatrixPos(screenpos));
	}
	const std::vector<Element>& GetMatrix() const
	{
		return Elements;
	}
public:
	RNG Chance;
	RNG Pick;
private:
	Dimensions<size_t> SandboxDim;
	std::vector<Element> Elements;

	std::vector<Move> Move_List;
	std::vector<Move> Fire_list;
	std::vector<Move> FireAura_list;
	std::vector<Move> Conversion_list;
	std::vector<Move> Spawn_list;

	Sprite BackGround = "background.bmp";
};

