#pragma once
#include "Element.h"
#include "Graphics.h"
#include <vector>
#include <algorithm>
#include "Dimensions.h"
#include "Rect.h"
#include "assert.h"
#include "Effects.h"
#include "Attributes.h"
#include <stdexcept>
#include "Camera.h"

class World {

public:
	static constexpr int ElemSize = 2;
	enum class MoveType : int8_t {
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
		Move(int elem_1, int elem_2, MoveType movetype)
			:elem1(elem_1), elem2(elem_2) , move(movetype)
		{
		}
		Move(int elem_1, int elem_2, MoveType movetype , Type conversion0)
			:elem1(elem_1), elem2(elem_2), move(movetype) , conversion(conversion0)
		{
		}
		Move(int elem_1, MoveType movetype, Type conversion0)
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
		int elem1 = 0;
		int elem2 = -1;
		MoveType move = MoveType::Static;
		Type conversion = Type::Empty;
	};
	struct SwapData {
		SwapData(int ind1 , int ind2)
			:index1(ind1) , index2(ind2)
		{}
		int index1;
		int index2;
		void Do(World& world) const
		{
			assert(index2 != -1);

			Element& elem_1 = *world.GetElem(index1);
			Element& elem_2 = *world.GetElem(index2);

			elem_1.SwapPositions(elem_2);
		}
		
	};
	struct Spawn {
		Spawn(int index0 , Type type0)
			:type(type0) , index(index0)
		{}
		void Do(World& world)
		{
			world.CreateElem(index, type);
		}
		Type type;
		int index;
	};
public:
	World()
	{

		const int width  = int(SandboxDim.width);
		const int height = int(SandboxDim.height);

		Elements = std::vector<Element>(width * height, Element(RectI(ElemSize, ElemSize, Vec2I(NULL, NULL))));

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				Vec2D Pos = Vec2D(float(x * ElemSize) + Graphics::WorldArea.left , float(y * ElemSize) + Graphics::WorldArea.bottom);
				Elements[y * width + x].Update(Pos);
			}
		}
		//blur.Blur(BackGround);
		
	}
	World(Sprite& sprite)
	{

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
	virtual void DrawWorld(Graphics& gfx , Camera& ct )
	{
		const int width = BackGround.GetWidth();
		const int height = BackGround.GetHeight();

		for (auto& n : Elements)
		{
			n.DrawElement(gfx , ct);
		}
	}

	static constexpr Dimensions<int> SandboxDim = Dimensions<int>( Graphics::WorldArea.width  / ElemSize , 
																   Graphics::WorldArea.height / ElemSize );

	static constexpr int SandSinkChance = 75;
	static constexpr int WoodFlamability = 50;


	Element* GetWorld()
	{
		return Elements.data(); 
	}
	std::vector<SwapData> GetMove_List()
	{
		return Move_List;
	}
	int GetMove_ListSize() const {
		return Move_List.size();
	}
	Dimensions<int> GetSandboxDim() const
	{
		return SandboxDim;
	}

	bool CreateElem(int index, Type type)
	{
		if (index >= 0 && index < Elements.size())
		{
			Elements[index].Create(type);
			return true;
		}
		return false;
	}

	void AddMoveToList(const SwapData& move)
	{
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
	void AddToSpawnList(Spawn& move) 
	{
		Spawn_list.emplace_back(std::move(move));
	}
	size_t GetFire_listSize() const
	{
		return Fire_list.size();
	}
	std::vector<Spawn> GetSpawn_list()
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
			move.Do(*this);
		}

		for (auto& n : Elements)
		{
			n.ResetStatus();
		}
		std::sort(Move_List.begin(), Move_List.end(), [](const SwapData& move1, const SwapData& move2)
			{
				return move1.index2 > move2.index2;
			});

		for (int ind = 0; ind < Move_List.size(); )
		{
			//im basically assuming there is no way 65000+ cells have the same place to go
			short add = 0;
		
			while (ind + add < Move_List.size())
			{
				if (Move_List[ind + add].index2 == Move_List[ind].index2)
					add++;
				else
					break;
			}
			if (add == 1)
			{
				Move_List[ind].Do(*this);
			}
			else
			{
				Pick.ChangeRange(ind, ind + add - 1);
				Move_List[Pick.GetVal()].Do(*this);
			}
			ind += add;
		}

		Fire_list.clear();
		Move_List.clear();
		FireAura_list.clear();
		Conversion_list.clear();
		Spawn_list.clear();
	}
	Element* GetElem(int index)
	{
		return &Elements[index];
	}
	Element* GetElem(Vec2I& pos)
	{
		return &Elements[(pos.y) * SandboxDim.width + pos.x];
	}

	Vec2I IndexToPos(int index)
	{
		return Vec2I(int(index % SandboxDim.width) , int(index / SandboxDim.width));
	}
	Vec2I ScreenToMatrixPos(Vec2I& pos)
	{
		//adding half of the sandbox dimensions because the matrix goes equaly in the - and + directions
		// 
		return Vec2I(pos.x / ElemSize + SandboxDim.width / 2, pos.y / ElemSize + SandboxDim.height / 2);
	}
	int MatrixPosToIndex(Vec2I& pos)
	{
		return pos.y * SandboxDim.width + pos.x;
	}
	int GetElemIndex(Vec2I& pos) const
	{
		return pos.y * SandboxDim.width + pos.x;
	}
	int GetElemIndScr(Vec2I& screenpos)
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
protected:
	std::vector<Element> Elements;

	std::vector<SwapData> Move_List;
	std::vector<Move> Fire_list;
	std::vector<Move> FireAura_list;
	std::vector<Move> Conversion_list;
	std::vector<Spawn> Spawn_list;

	Sprite BackGround = "background.bmp";
};

struct MoveableElement {
	MoveableElement(Element& element, Vec2D vel0)
	{
		atr.GetAttributes(element);
		RectI elem_hitbox = element.GetRect();
		HitBox = Rect(elem_hitbox.width, elem_hitbox.height, Vec2D(elem_hitbox.left, elem_hitbox.bottom));
		vel = std::move(vel0);

		Convert = false;
	}
	void Draw(Graphics& gfx , Camera& ct)
	{
		gfx.DrawRect(RectI(HitBox.GetDimensions() , ct.Transform(HitBox.GetPos())), Colors::Magenta, Effects::Copy{}, gfx.GetScreenRect());
	}
	Rect HitBox;
	Attributes atr;
	Vec2D vel;

	bool Convert = false;
};

class Simulation : public World {
public:
	void Go(float time)
	{
		for (auto& elem : particle_list)
		{
			if (elem.Convert)
			{
				continue;
			}

			Vec2D& vel = elem.vel;
			Vec2D pos = elem.HitBox.GetPos();

			if (vel.x > 0)
			{
				MoveRight(elem, time, vel.x);
				elem.Convert = elem.HitBox.left == pos.x;
			}
			else if (vel.x < 0)
			{
				MoveLeft(elem, time, vel.x);
				elem.Convert = elem.HitBox.left == pos.x;
			}


			if (vel.y > 0)
			{
				MoveUp(elem, time, vel.y);
				elem.Convert = elem.HitBox.bottom == pos.y;
			}
			else if (vel.y < 0)
			{
				MoveDown(elem, time, vel.y);
				elem.Convert = elem.HitBox.bottom == pos.y;
			}

			if (elem.Convert)
			{
				continue;
			}

			vel.y -= Gravity * time * 60.0f;

			Vec2D new_pos = elem.HitBox.GetPos();

		}

		particle_list.remove_if([=](MoveableElement& elem) {
			if (elem.Convert)
			{
				PutIntoMatrix(elem);
				return true;
			}
			return false;
			});
	}

	void MoveRight(MoveableElement& elem, float time, float vel)
	{
		float AddX = vel * time * 60.0f;
		Rect& HitBox = elem.HitBox;

		//basically i am getting the modulus for a float 
		float offset = HitBox.left - (int(HitBox.left) / World::ElemSize) * World::ElemSize;
		int LineX = HitBox.right();
		if (offset != 0.0f)
		{
			LineX += World::ElemSize;
		}

		while (AddX)
		{
			if (LineX >= Graphics::WorldArea.right())
			{
				break;
			}
			bool Move = true;
			std::vector<State> elem_list;
			for (int y = HitBox.bottom; y < HitBox.top(); y += World::ElemSize)
			{
				Vec2I pos = ScreenToMatrixPos(Vec2I(LineX, y));
				int index = pos.y * World::SandboxDim.width + pos.x;
				elem_list.emplace_back(GetElem(index)->GetState());
			}

			for (int ind = 0; ind < elem_list.size(); ind++)
			{
				if (elem_list[ind] == State::Solid)
				{
					{
						Move = false;
						break;
					}
				}
			}

			if (Move == true)
			{
				if (AddX < World::ElemSize)
				{
					HitBox.left += AddX;
					AddX = 0;
				}
				else
				{
					HitBox.left += World::ElemSize;
					AddX -= World::ElemSize;
				}
			}
			else
			{
				//distance to the next cell
				float dist = World::ElemSize - offset;

				if (offset != 0.0f)
				{
					if (AddX < dist)
					{
						HitBox.left += AddX;
						break;
					}
					else
					{
						HitBox.left += dist;
						break;
					}
				}

				break;
			}

			LineX += World::ElemSize;
		}
	}

	void MoveLeft(MoveableElement& elem, float time, float vel)
	{
		float AddX = std::abs(vel * time * 60.0f);
		int LineX = elem.HitBox.left - World::ElemSize;
		while (AddX)
		{
			if (LineX < Graphics::WorldArea.left)
			{
				break;
			}
			bool Move = true;
			std::vector<State> elem_list;
			for (int y = elem.HitBox.bottom; y < elem.HitBox.top(); y += World::ElemSize)
			{
				Vec2I pos = ScreenToMatrixPos(Vec2I(LineX, y));
				int index = pos.y * World::SandboxDim.width + pos.x;
				elem_list.emplace_back(GetElem(index)->GetState());
			}

			for (int ind = 0; ind < elem_list.size(); ind++)
			{
				if (elem_list[ind] == State::Solid)
				{
					if (ind > elem_list.size() - 5)
					{
						elem.HitBox.bottom -= (ind - (elem_list.size() - 5)) * World::ElemSize;
					}
					else
					{
						Move = false;
						break;
					}
				}
			}

			if (Move == true)
			{
				if (AddX < World::ElemSize)
				{
					elem.HitBox.left -= AddX;
					AddX = 0;
				}
				else
				{
					elem.HitBox.left -= World::ElemSize;
					AddX -= World::ElemSize;
				}
			}
			else
			{
				//basically i am getting the modulus for a float 
				//distance to the next cell
				float dist = elem.HitBox.left - (int(elem.HitBox.left) / World::ElemSize) * World::ElemSize;

				if (AddX < dist)
				{
					elem.HitBox.left -= AddX;
					break;
				}
				else
				{
					elem.HitBox.left -= dist;
					break;
				}
			}

			LineX -= World::ElemSize;
		}
	}

	void MoveUp(MoveableElement& elem, float time, float vel)
	{
		float AddY = vel * time * 60.0f;
		//basically i am getting the modulus for a float 
		float offset = elem.HitBox.bottom - (int(elem.HitBox.bottom) / World::ElemSize) * World::ElemSize;

		int LineY = elem.HitBox.top();
		if (offset != 0.0f)
		{
			LineY += World::ElemSize;
		}

		while (AddY)
		{
			if (LineY >= Graphics::WorldArea.top())
			{
				break;
			}
			bool Move = true;
			std::vector<State> elem_list;
			for (int x = elem.HitBox.left; x < elem.HitBox.right(); x += World::ElemSize)
			{
				Vec2I pos = ScreenToMatrixPos(Vec2I(x, LineY));
				int index = pos.y * World::SandboxDim.width + pos.x;
				elem_list.emplace_back(GetElem(index)->GetState());
			}

			for (int ind = 0; ind < elem_list.size(); ind++)
			{
				if (elem_list[ind] == State::Solid)
				{
					Move = false;
					break;
				}
			}

			if (Move == true)
			{
				if (AddY < World::ElemSize)
				{
					elem.HitBox.bottom += AddY;
					AddY = 0;
				}
				else
				{
					elem.HitBox.bottom += World::ElemSize;
					AddY -= World::ElemSize;
				}
			}
			else
			{
				//distance to the next cell
				float dist = World::ElemSize - offset;

				if (offset != 0.0f)
				{
					if (AddY < dist)
					{
						elem.HitBox.bottom += AddY;
						break;
					}
					else
					{
						elem.HitBox.bottom += dist;
						break;
					}
				}

				break;
			}

			LineY += World::ElemSize;
		}
	}

	void MoveDown(MoveableElement& elem, float time, float vel)
	{
		float AddY = std::abs(vel * time * 60.0f);
		int LineY = elem.HitBox.bottom - World::ElemSize;
		bool HitObstacle = false;

		while (AddY)
		{
			if (LineY < Graphics::WorldArea.bottom)
			{
				break;
			}
			bool Move = true;
			std::vector<State> elem_list;
			for (int x = elem.HitBox.left; x < elem.HitBox.right(); x += World::ElemSize)
			{
				Vec2I pos = ScreenToMatrixPos(Vec2I(x, LineY));
				int index = pos.y * World::SandboxDim.width + pos.x;
				elem_list.emplace_back(GetElem(index)->GetState());
			}

			for (int ind = 0; ind < elem_list.size(); ind++)
			{
				if (elem_list[ind] == State::Solid)
				{
					Move = false;
					break;
				}
			}

			if (Move == true)
			{
				if (AddY < World::ElemSize)
				{
					elem.HitBox.bottom -= AddY;
					AddY = 0;
				}
				else
				{
					elem.HitBox.bottom -= World::ElemSize;
					AddY -= World::ElemSize;
				}
			}
			else
			{
				//basically i am getting the modulus for a float 
				//distance to the next cell
				float dist = elem.HitBox.bottom - int(int(elem.HitBox.bottom) / World::ElemSize) * World::ElemSize;

				if (AddY < dist)
				{
					elem.HitBox.bottom -= AddY;
					break;
				}
				else
				{
					elem.HitBox.bottom -= dist;
					break;
				}
			}

			LineY -= World::ElemSize;
		}
	}

	void AddToList(MoveableElement elem)
	{
		particle_list.emplace_front(std::move(elem));
	}

	void PutIntoMatrix(const MoveableElement& elem)
	{
		auto pos = elem.HitBox.GetPos();
		Vec2I scr_pos = Vec2I(std::move(pos.x), std::move(pos.y));
		Vec2I matrix_pos = ScreenToMatrixPos(std::move(scr_pos));

		int index = matrix_pos.y * World::SandboxDim.width + matrix_pos.x;

		Element* matrix_element = GetElem(index);

		elem.atr.PassAttributes(*matrix_element);
	}
	void DrawWorld(Graphics& gfx , Camera& ct) override {
		World::DrawWorld(gfx , ct);
		for (auto& particle : particle_list)
		{
			particle.Draw(gfx , ct);
		}
	}
	static constexpr float Gravity = 0.5f;
private:
	std::forward_list<MoveableElement> particle_list;
	RNG power_rng = { 0, 1 };
};
