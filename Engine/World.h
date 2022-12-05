#pragma once
#include "Element.h"
#include "Graphics.h"
#include <vector>
#include <algorithm>
#include "Dimensions.h"
#include "Rect.h"
#include "assert.h"
#include "Effects.h"
#include <stdexcept>

class World {
public:
	enum class MoveType {
		Conversion,
		SetOnFire,
		FireAura,
		Static,
		Special,
		Create,
		Swap,
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
		void SetOnFire(World& world)
		{
			world.GetElem(elem1)->SetOnFire();
		}
		void EmitAura(World& world)
		{
			world.GetElem(elem1)->Create(Type::FireAura);
		}
		void Spawn(World& world)
		{
			world.CreateElem(elem1, conversion);
		}
		size_t elem1 = 0;
		int elem2 = -1;
		MoveType move = MoveType::Static;
		Type conversion = Type::Empty;
	};
	struct Swap {
		Swap(int ind1 , int ind2)
			:index1(ind1) , index2(ind2)
		{}
		int index1;
		int index2;
		void operator()(World& world)
		{
			assert(elem2 != -1);

			Element& elem_1 = *world.GetElem(index1);
			Element& elem_2 = *world.GetElem(index2);

			elem_1.SwapPositions(elem_2);
		}
		
	};
public:
	World()
	{
		SandboxDim.width = Graphics::ScreenWidth / ElemSize;
		SandboxDim.height = Graphics::ScreenHeight / ElemSize;

		const int width  = int(SandboxDim.width);
		const int height = int(SandboxDim.height);

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
	World(Sprite& sprite)
	{
		SandboxDim.width = Graphics::ScreenWidth / ElemSize;
		SandboxDim.height = Graphics::ScreenHeight / ElemSize;

		const int width = int(SandboxDim.width);
		const int height = int(SandboxDim.height);

		Elements = std::vector<Element>(width * height, Element(RectI(ElemSize, ElemSize, Vec2I(NULL, NULL))));

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				size_t index = y * width + x;

				Color c = sprite.GetPixel((x * ElemSize), (y * ElemSize));

				Vec2D Pos = Vec2D(float(x * ElemSize), float(y * ElemSize));

				auto type = ConvertPixel(c);
	
				Elements[index].Update(Pos);
				Elements[index].Create(type);
			}
		}
	}
	Type ConvertPixel(Color& c)
	{
		std::vector<short> acurracy_list;
		acurracy_list.resize(size_t(Type::Count) - 3);
		//the last 3 elements are not really usefull to match/can't match

		acurracy_list[0] = c.Match(Element::WaterColorRange   [0]);
		acurracy_list[1] = c.Match(Element::SandColorRange    [0]);
		acurracy_list[2] = c.Match(Element::StoneColorRange   [0]);
		acurracy_list[3] = c.Match(Element::WoodColorRange    [0]);
		acurracy_list[4] = c.Match(Element::FireColorRange    [0]);
		acurracy_list[5] = c.Match(Element::SmokeColorRange   [0]);
		acurracy_list[6] = c.Match(Element::SteamColorRange   [0]);
		acurracy_list[7] = c.Match(Element::SnowColorRange    [0]);
		acurracy_list[8] = c.Match(Element::AcidColorRange    [0]);
		acurracy_list[9] = c.Match(Element::ToxicGasColorRange[0]);

		struct {
			unsigned char max = 0;
			short index = 0;
		} pick;
		for (size_t ind = 0 ; ind < acurracy_list.size() ; ind++)
		{
			if (pick.max < acurracy_list[ind])
			{
				pick.max   = unsigned char(acurracy_list[ind]);
				pick.index = short(ind);
			}
		}

		return Type(pick.index);
	}
	void DrawWorld(Graphics& gfx)
	{
		const int width = BackGround.GetWidth();
		const int height = BackGround.GetHeight();

		Effects::Copy e{};

		for (auto& n : Elements)
		{
			n.DrawElement(gfx , BackGround);
		}
	}

	static constexpr int ElemSize = 2;

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
		assert(move.move == MoveType::Swap);
		Move_List.emplace_back(std::move(move));
	}
	void AddToFireList(Move& move)
	{
		Fire_list.emplace_back(std::move(move));
	}
	void AddToFireAuraList(Move& move)
	{
		FireAura_list.emplace_back(std::move(move));
	}
	void AddToConversionList(Move& move)
	{
		Conversion_list.emplace_back(std::move(move));
	}
	void AddToSpawnList(Move& move) 
	{
		Spawn_list.emplace_back(std::move(move));
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
			move.SetOnFire(*this);
		}
		for (auto& move : FireAura_list)
		{
			move.EmitAura(*this);
		}
		for (auto& move : Conversion_list)
		{
			move.Spawn(*this);
		}
		for (auto& move : Spawn_list)
		{
			move.Spawn(*this);
		}

		for (auto& n : Elements)
		{
			n.ResetStatus();
		}
		std::sort(Move_List.begin(), Move_List.end(), [](Move& move1, Move& move2)
			{
				return move1.elem2 > move2.elem2;
			});

		for (int ind = 0; ind < Move_List.size(); )
		{
			//im basically assuming there is no way 65000+ cells have the same place to go
			short add = 0;

			while (ind + add < Move_List.size())
			{
				if (Move_List[ind + add].elem2 == Move_List[ind].elem2)
					add++;
				else
					break;
			}
			if (add == 1)
			{
				Move_List[ind].MoveElem(*this);
			}
			else
			{
				Pick.ChangeRange(ind, ind + add - 1);
				Move_List[Pick.GetVal()].MoveElem(*this);
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
	Element* GetElem(Vec2I& pos)
	{
		return &Elements[pos.y * SandboxDim.width + pos.x];
	}

	Vec2I IndexToPos(size_t index)
	{
		return Vec2I(int(index % SandboxDim.width) , int(index / SandboxDim.width ));
	}
	Vec2I ScreenToMatrixPos(Vec2I& pos)
	{
		return Vec2I(pos.x / ElemSize , pos.y / ElemSize);
	}
	size_t MatrixPosToIndex(Vec2I& pos)
	{
		return pos.y * SandboxDim.width + pos.x;
	}
	size_t GetElemIndex(Vec2I& pos) const
	{
		return pos.y * SandboxDim.width + pos.x;
	}
	size_t GetElemIndScr(Vec2I& screenpos)
	{
		return GetElemIndex(ScreenToMatrixPos(screenpos));
	}
	const std::vector<Element>& GetMatrix() const
	{
		return Elements; 
	}
public:
	RNG Chance = { 1 , 100};
	RNG Pick   = { 0 , 2 };
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

