#pragma once

#include <vector>
#include "engine2d/math/Rng.hpp"
#include "engine2d/pipeline/Camera.hpp"
#include "engine2d/math/Rect.hpp"
#include "engine2d/Timer.hpp"
#include "engine2d/core/Graphics.hpp"
#include "falling-sand/ParticleEffect.hpp"


namespace sand {
	enum class Type : int8_t {
		Water,
		Sand,
		Stone,
		Metal,
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
		None,
		Count
	};
	enum class State : int8_t {
		Liquid,
		Solid,
		Gas,
		Plasma,
		Empty,
		Count,
		Invalid
	};

	class Attributes;

	class Element {
		friend class Attributes;
	public:
		static constexpr chili::Color SandColorRange[3] = { { 255 , 220 , 0 } ,
													 { 255 , 205 , 0 } ,
													 { 255 , 190 , 0 } };

		static constexpr chili::Color WaterColorRange[3] = { { 0, 115, 189 } ,
													  { 0, 115, 206} ,
													  { 0, 115, 220} };


		static constexpr chili::Color WoodColorRange[3] = { { 120, 86, 2 } ,
													 { 153, 100, 2 } ,
													 { 180, 129, 43 } };

		static constexpr chili::Color StoneColorRange[3] = { { 100 , 100 , 100 } ,
													  { 110 , 110 , 110 } ,
													  { 120 , 120 , 120 } };

		static constexpr chili::Color FireColorRange[3] = { { 225 , 50 , 0 } ,
													 { 255 , 100 , 0 } ,
													 { 255 , 150 , 0 } };

		static constexpr chili::Color FireAuraColorRange[3] = { { 112 , 25 , 0 } ,
														 { 227 , 50 , 0 } ,
														 { 227 , 75 , 0 } };

		static constexpr chili::Color SmokeColorRange[3] = { { 35 , 35 , 35} ,
													  { 45 , 45 , 45} ,
													  { 55 , 55 , 55} };

		static constexpr chili::Color SteamColorRange[3] = { { 240 , 240 , 240 },
														{230 , 230 , 230 },
														{220 , 220 , 220} };

		static constexpr chili::Color SnowColorRange[3] = { { 255 , 255 , 255   },
														{245 , 245 , 245 },
														{240 , 240 , 240} };
		static constexpr chili::Color AcidColorRange[3] = { { 0 , 150 , 0 },
													  {0 , 155 , 0 },
													  {0 , 160 , 0} };
		static constexpr chili::Color ToxicGasColorRange[3] = { {0 , 150 , 0},
													   {0 , 155 , 0 },
													   {0 , 160 , 0} };
		static constexpr chili::Color MetalColorRange[3] = { { 130 , 130 , 130} ,
													 { 150 , 150 , 150} ,
													 { 175 , 175 , 175} };

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
		  {std::begin(Conditions_gas)    , std::end(Conditions_gas)} };

	public:
		static constexpr short WaterSpread = 4;
		static constexpr short SandSinkChance = 15;
		static constexpr short SnowToWaterChance = 10;
		static constexpr short SnowSinkChance = 10;

		static constexpr int AcidDestroyChance = 30;
	public:
		Element(const e2d::RectI& rect);
		void DrawElement(e2d::Renderer& gfx, const e2d::Camera& ct);
		void Update(e2d::Vec2D newpos);
		void SetVel(e2d::Vec2D newvel);
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
		e2d::RectI GetRect() const;
		e2d::Vec2D GetVel() const;
		unsigned char GetBurnChance() const;
		bool IsFlammable() const;
		bool IsEmpty() const;

		const e2d::Countdown& GetLifeSpan() const;
		const e2d::Countdown& GetColorTimer() const;
		chili::Color RandColor(const chili::Color* range);

		chili::Color GetColor() const;
		void SetColor(chili::Color c);
		void SetLifeSpan(const e2d::Countdown timer);
		void SetColorTimer(const e2d::Countdown timer);

		bool Update_Fire(float time);
		bool CaughtOnFire();
		void Update_Steam(float time);
		size_t GetChunkIndex() const;

		void AssignChunk(size_t index);
		chili::Color DetermineGasColor() const;
		bool CanMove(const Element& elem) const;
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
		// will be removed later by using a simulation renderer
		e2d::RectI hBox;

		e2d::Vec2D vel;

		/* this is the core */
		Type type;
		State state;
		chili::Color color;
		/********************/

		// will be removed later
		unsigned char BurnChance;
		float BurnDuration;

		e2d::Countdown LifeSpan;
		e2d::Countdown ChangeColor;

		// might remove later
		size_t ChunkIndex = 0;

		// somehow remove this?
		bool Updated = false;

		// I have TO remove this later bruv
		e2d::RNG RdLifeSpan_gas;
		e2d::RNG ColorRng;
		e2d::RNG XRange;
		mutable e2d::RNG SpreadRange;
	};
}