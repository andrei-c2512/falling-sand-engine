#include "Element.hpp"
#include <assert.h>
#include "SpecialBehaviour.hpp"
#include <iterator>

namespace sand {
	Element::Element(const e2d::RectI& rect)
		:hBox(rect), RdLifeSpan_gas(10, 15), ColorRng(0, 2), SpreadRange(-2, 2), XRange(0, hBox.width - 1)
	{
		type = Type::Empty;
		state = State::Empty;
		vel = { 1.0f , 1.0f };
		BurnChance = 0;
		ChangeColor = { 0.0f };
		LifeSpan = { 0.0f };
		BurnDuration = { 0.0f };
	}
	void Element::Update(e2d::Vec2D newpos)
	{
		hBox.left = newpos.x;
		hBox.bottom = newpos.y;
	}

	Type Element::GetType() const {
		return type;
	}

	e2d::RectI Element::GetRect() const
	{
		return hBox;
	}

	e2d::Vec2D Element::GetVel() const {
		return vel;
	}

	void Element::SwapPositions(Element& elem) {
		using namespace std;
		//assert(SwapCnt == 0 && elem.SwapCnt == 0);
		swap(type, elem.type);
		swap(color, elem.color);
		swap(BurnDuration, elem.BurnDuration);
		swap(BurnChance, elem.BurnChance);
		swap(elem.LifeSpan, LifeSpan);
		swap(elem.state, state);
		swap(elem.Updated, Updated);
	}

	void Element::SetType(const Type newtype)
	{
		type = newtype;
	}

	void Element::SetVel(e2d::Vec2D newvel)
	{
		vel = newvel;
	}
	void Element::Create(const Type newtype)
	{
		type = newtype;
		switch (type)
		{
		case Type::Sand:
			color = RandColor(&SandColorRange[0]);
			BurnChance = 0;
			state = State::Solid;
			break;
		case Type::Water:
			color = chili::Colors::Cyan;
			BurnChance = 0;
			state = State::Liquid;
			break;
		case Type::Stone:
			color = RandColor(&StoneColorRange[0]);
			BurnChance = 0;
			state = State::Solid;
			break;
		case Type::Metal:
			color = RandColor(&MetalColorRange[0]);
			BurnChance = 0;
			state = State::Solid;
			break;
		case Type::Wood:
			color = RandColor(&WoodColorRange[0]);
			BurnChance = WoodBurnChance;
			LifeSpan = { 1.0f };
			state = State::Solid;
			BurnDuration = { 2.5f };
			break;
		case Type::Fire:
			color = RandColor(&FireColorRange[0]);
			LifeSpan = { 0.1f };
			ChangeColor = { 0.01f };
			state = State::Plasma;
			BurnChance = 0;
			break;
		case Type::FireAura:
			color = RandColor(&FireAuraColorRange[0]);
			LifeSpan = { 0.01f };
			ChangeColor = { 0.01f };
			state = State::Plasma;
			BurnChance = 0;
			break;
		case Type::Smoke:
			color = RandColor(&SmokeColorRange[0]);
			LifeSpan = RdLifeSpan_gas.GetVal();
			BurnChance = 0;
			state = State::Gas;
			break;
		case Type::Steam:
			color = RandColor(&SteamColorRange[0]);
			LifeSpan = RdLifeSpan_gas.GetVal();
			BurnChance = 0;
			state = State::Gas;
			break;
		case Type::Snow:
			color = RandColor(&SnowColorRange[0]);;
			BurnChance = 0;
			state = State::Solid;
			break;
		case Type::Acid:
			color = RandColor(&AcidColorRange[0]);
			BurnChance = 0;
			state = State::Liquid;
			break;
		case Type::ToxicGas:
			color = RandColor(&ToxicGasColorRange[0]);
			LifeSpan = RdLifeSpan_gas.GetVal();
			BurnChance = 100;
			state = State::Gas;
			BurnDuration = { 0.1f };
			break;
		default:
			color = chili::Colors::Black;
			LifeSpan = { 0.0f };
			state = State::Empty;
			BurnChance = 0;
			BurnDuration = { 0.0f };
			break;
		}
	}

	void Element::DrawElement(e2d::Renderer& gfx, const e2d::Camera& cam)
	{
		if (state != State::Empty)
		{
			e2d::Vec2I pos = cam.Transform(hBox.GetPos());
			e2d::RectI new_rect = e2d::RectI(hBox.GetSize(), std::move(pos));
			if (type == Type::Water)
			{
				gfx.DrawRect(new_rect.left, new_rect.bottom, new_rect.width, new_rect.height, color);
			}
			else if (state == State::Gas)
			{
				gfx.DrawRect(new_rect.left, new_rect.bottom, new_rect.width, new_rect.height, DetermineGasColor());
			}
			else if (type == Type::Acid || state == State::Plasma)
			{
				//gfx.DrawRectI_Bloom(std::move(new_rect), color);
				gfx.DrawElementRect(std::move(new_rect), color);
			}
			else
				gfx.DrawElementRect(std::move(new_rect), color);
		}
	}

	bool Element::IsFlammable() const {
		return BurnChance > 0;
	}

	chili::Color Element::GetColor() const { return color; }

	void Element::SetColor(chili::Color c) { color = std::move(c); }

	chili::Color Element::RandColor(const chili::Color* range)
	{
		return range[ColorRng.GetVal()];
	}

	void Element::UpdateColor()
	{
		chili::Color c;
		switch (type)
		{
		case Type::Fire:
			c = color;
			do {
				color = RandColor(&FireColorRange[0]);
			} while (c == color);
			break;
		case Type::FireAura:
			c = color;
			do {
				color = RandColor(&FireAuraColorRange[0]);
			} while (c == color);
			break;
		default:
			color = chili::Colors::Black;
			break;
		}
	}

	void Element::SetOnFire()
	{
		type = Type::Fire;
		state = State::Plasma;
		LifeSpan = { BurnDuration };
		ChangeColor = { 0.01f };
		BurnChance = 0;
		UpdateColor();
	}


	bool Element::UpdateLifeSpan(float time)
	{
		//assert(LifeSpan != Timer(0.0f));
		LifeSpan.Update(time);
		if (LifeSpan.IsReady())
		{
			// LifeSpan.ResetTimer();

			//if(type == Type::Fire || type == Type::FireAura)
			{
				assert(color != chili::Colors::Black);
				Create(Type::Empty);
			}
			return false;
		}
		return true;
	}

	void Element::UpdateColorTime(float time)
	{
		ChangeColor.Update(time);
		if (ChangeColor.IsReady() == true)
		{
			ChangeColor.SetTime(0.01f);
			UpdateColor();
		}
	}

	unsigned char Element::GetBurnChance() const {
		return BurnChance;

	}

	void Element::SwapPositions_Gas(Element& elem)
	{
		SwapPositions(elem);
	}

	void Element::SetLifeSpan(const e2d::Countdown timer)
	{
		LifeSpan = timer;
	}

	void Element::SetColorTimer(const e2d::Countdown timer)
	{
		ChangeColor = timer;
	}

	const e2d::Countdown& Element::GetLifeSpan() const {
		return LifeSpan;
	}

	const e2d::Countdown& Element::GetColorTimer() const {
		return ChangeColor;
	}

	bool Element::Update_Fire(float time)
	{
		{
			UpdateColorTime(time);
		}

		if (UpdateLifeSpan(time) == false)
		{
			if (e2d::RNG::RandomPercentile() > 95) // chance to convert to smoke (100 - x)
			{
				Create(Type::Smoke);
			}
		}
		return true;
	}

	void Element::Update_Steam(float time)
	{
		if (UpdateLifeSpan(time) == false)
		{
			if (e2d::RNG::RandomPercentile() > 90) // chance to convert towater(100 - x) 
			{
				Create(Type::Water);
			}
		}
	}
	size_t Element::GetChunkIndex() const {
		return ChunkIndex;
	}

	void Element::AssignChunk(size_t index)
	{
		ChunkIndex = index;
	}

	bool Element::IsEmpty() const {
		return type == Type::Empty;
	}

	void Element::MergeElem(Element& elem, const Type merging)
	{
		elem.Create(Type::Empty);
		SwapPositions(elem);
		Create(merging);
	}

	bool Element::CanMove(const Element& elem) const
	{
		State type = elem.GetState();

		bool Move = false;

		auto Range = CondList[int(state)];
		for (; Range.first != Range.second; Range.first++)
		{
			Move = Move || (*Range.first == type);
		}

		return Move;
	}

	State Element::GetState() const
	{
		return state;
	}

	bool Element::CaughtOnFire()
	{
		return e2d::RNG::RandomPercentile() < BurnChance;
	}

	std::pair<const State*, const State*> Element::GetConditions() const
	{
		std::pair<const State*, const State*> result = { nullptr , nullptr };

		switch (state)
		{
		case State::Solid:
			result = { std::begin(Conditions_solid) ,std::end(Conditions_solid) };
			break;
		case State::Liquid:
			result = { std::begin(Conditions_liquid) ,std::end(Conditions_liquid) };
			break;
		case State::Gas:
			result = { std::begin(Conditions_gas) ,std::end(Conditions_gas) };
			break;
		default:
			result = { nullptr , nullptr };
			break;
		}

		return result;
	}

	void Element::Explode(ParticleEffect& list)
	{
		if (e2d::RNG::RandomPercentile() <= 20)
		{
			unsigned char add = 0;
			unsigned char ParticleSize = 4;
			// make sure not to spawn beyond screen
			do {
				add = XRange.GetVal();
			} while (hBox.left + add + 4 > e2d::Renderer::ScreenWidth);

			auto particle = Particle(e2d::RectI(ParticleSize, ParticleSize, e2d::Vec2I(hBox.left + add, hBox.bottom))
				, RandColor(&FireColorRange[0]), e2d::Vec2D{ 0.0f , -4.0f }, e2d::Timer(0.3f));

			list.AddParticle(std::move(particle));

		}

		Create(Type::Empty);
	}

	void Element::Darken(int percentage)
	{
		if (state == State::Solid)
		{
			unsigned char r, g, b;
			r = float(color.GetR()) * float(percentage) / 100.0f;
			g = float(color.GetG()) * float(percentage) / 100.0f;
			b = float(color.GetB()) * float(percentage) / 100.0f;

			if (!(r < 30 && g < 30 && b < 30))
				color = chili::Color(r, g, b);
		}
	}

	chili::Color Element::DetermineGasColor() const {
		assert(state == State::Gas);
		if (LifeSpan.GetTime() >= LifeSpan.GetTimeLimit() - 2.0f)
		{
			float color_fade = 1.0f - LifeSpan.GetTime() / LifeSpan.GetTimeLimit();

			unsigned char r = color.GetR() * color_fade,
				g = color.GetG() * color_fade,
				b = color.GetB() * color_fade;

			return chili::Color(r, g, b);
		}
		else
		{
			return color;
		}
	}
}