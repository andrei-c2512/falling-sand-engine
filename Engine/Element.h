#pragma once

#include <vector>
#include "Rng.h"
#include "ParticleEffect.h"

extern RNG Chance;

enum class Type {
	Water,
	Sand,
	Stone,
	Wood,
	Fire,
	Smoke,
	Steam,
	Snow,
	Acid,
	ToxicGas,
	Empty,
	Explosion,
	FireAura,
	Count
};
enum class State {
	Liquid,
	Solid,
	Gas,
	Plasma,
	Empty,
	Count,
	Invalid
};

enum class Action {
	Explode,
	Darken,
	None,
	Count
};
class Element {

public:
	static constexpr Color SandColorRange[3] = { { 255 , 220 , 0 } ,
												 { 255 , 205 , 0 } ,
												 { 255 , 190 , 0 } };

	static constexpr Color WaterColorRange[3] = { { 0, 115, 189 } ,
												  { 0, 115, 206} ,
												  { 0, 115, 220} };


	static constexpr Color WoodColorRange[3] = { { 120, 86, 2 } ,
												 { 153, 100, 2 } ,
												 { 180, 129, 43 } };

	static constexpr Color StoneColorRange[3] = { { 100 , 100 , 100 } ,
												  { 110 , 110 , 110 } ,
												  { 120 , 120 , 120 } };

	static constexpr Color FireColorRange[3] = { { 225 , 50 , 0 } ,
												 { 255 , 100 , 0 } ,
												 { 255 , 150 , 0 } };

	static constexpr Color FireAuraColorRange[3] = { { 112 , 25 , 0 } ,
													 { 227 , 50 , 0 } ,
													 { 227 , 75 , 0 } };

	static constexpr Color SmokeColorRange[3] = { { 35 , 35 , 35} ,
												  { 45 , 45 , 45} ,
												  { 55 , 55 , 55} };

	static constexpr Color SteamColorRange[3] = { { 240 , 240 , 240 },
													{230 , 230 , 230 },
													{220 , 220 , 220} };

	static constexpr Color SnowColorRange[3] = { { 255 , 255 , 255   },
													{245 , 245 , 245 },
													{240 , 240 , 240} };
	static constexpr Color AcidColorRange[3] = { { 0 , 150 , 0 },
												  {0 , 155 , 0 },
												  {0 , 160 , 0} };
	static constexpr Color ToxicGasColorRange[3] = { {0 , 150 , 0},
												   {0 , 155 , 0 },
												   {0 , 160 , 0} };

//public:
//	struct ElemInfo {
//		ElemInfo( short spread0 , short gravity0 , short burnchance , float BurnDuration0)
//			:spread(spread0),gravity(gravity0),BurnChance(burnchance),BurnDuration(BurnDuration0)
//		{}
//
//		short spread = 0;
//		short gravity = 0;
//		unsigned char BurnChance = 0;
//		float BurnDuration = 0.0f;
//	};
//public:
//	static ElemInfo elemtype_info[int(Type::Count)] = {
//		ElemInfo()
//	};
public:
	static constexpr short WoodBurnChance = 3;

	static constexpr State Conditions_liquid[] = { State::Empty , State::Plasma , State::Gas };
	static constexpr State Conditions_solid[] = { State::Empty , State::Liquid , State::Gas , State::Plasma };
	static constexpr State Conditions_gas[] = { State::Empty };

	static constexpr std::pair<const State*, const State*> CondList[] =
	{ {std::begin(Conditions_liquid) , std::end(Conditions_liquid)} ,
	  {std::begin(Conditions_solid)  , std::end(Conditions_solid)} ,
	  {std::begin(Conditions_gas)    , std::end(Conditions_gas)}};

public:
	static constexpr short WaterSpread = 4;
	static constexpr short SandSinkChance = 15;
	static constexpr short SnowToWaterChance = 10;
	static constexpr short SnowSinkChance = 10;

	static constexpr int AcidDestroyChance = 30;
public:
	Element(RectI& rect);
	void DrawElement(Graphics& gfx, Sprite& background);
	void Update(Vec2D newpos);
	void SetVel(Vec2D newvel);
	void SwapPositions(Element& elem);
	void SwapPositions_Gas(Element& elem);

	void MergeElem(Element& elem, Type merging);

	void Explode(ParticleEffect& list);
	void Darken(int percentage);

	void Create(Type newtype);
	void SetType(Type newtype);
	void SetOnFire();

	bool UpdateLifeSpan(float time);
	void UpdateColorTime(float time);
	void UpdateColor(); //if the elem is the type to change color periodically

	Type GetType() const;
	State GetState() const;
	RectI GetRect() const;
	Vec2D GetVel() const;
	unsigned char GetBurnChance() const;
	bool IsFlammable() const;
	bool IsEmpty() const;

	Timer GetLifeSpan() const;
	Timer GetColorTimer() const;
	Color RandColor(const Color* range);

	Color GetColor() const;
	void SetColor(Color c);
	void SetLifeSpan(Timer& timer);
	void SetColorTimer(Timer& timer);

	bool Update_Fire(float time);
	bool CaughtOnFire();
	void Update_Steam(float time);
	size_t GetChunkIndex() const;

	void AssignChunk(size_t index);
	bool CanMove(Element& elem) const;
	std::pair<const State*, const State*> GetConditions() const;
	void ResetStatus()
	{
		Updated = false;
	}
	void Update()
	{
		Updated = true;
	}
	bool IsUpdated() const {
		return Updated;
	}
private:
	RectI hBox;
	Vec2D vel;

	Type type;
	State state;

	Color color;

	unsigned char BurnChance;
	float BurnDuration;

	Timer LifeSpan;
	Timer ChangeColor;

	size_t ChunkIndex = 0;

	bool Updated = false;
private:
	RNG RdLifeSpan_gas = { 10 , 15 };
	RNG ColorRng = { 0 , 3 };
	RNG XRange = { 0 , hBox.width - 1 };
	mutable RNG SpreadRange = { -2 , 2 };
};