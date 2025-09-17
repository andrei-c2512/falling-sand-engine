#pragma once
#include "World.hpp"

class Action {
public:
	Action(int ind)
		:index(ind)
	{}
	virtual void Apply(Element& element) {};
public:
	int index;
};

class Explode :public Action{
public:
	Explode(int ind , Vec2D vel0 , Simulation& sim , ParticleEffect& effect0)
		:Action(ind), vel(std::move(vel0)) , simulation(sim), effect(effect0)
	{}
	void Apply(Element& element) override
	{
		element.Explode(effect);
		auto state = element.GetState();
	//	if(state == State::Liquid || state == State::Solid)
		{
			MoveableElement new_elem(element, vel);

			simulation.AddToList(std::move(new_elem));
		}
	};
private:
	Vec2D vel = { 0.0f , 0.0f };
	Simulation& simulation;
	ParticleEffect& effect;
};

class Darken :public Action {
public:
	static constexpr unsigned char DarkeningPercentage = 80;

	Darken(int ind)
		:Action(ind)
	{
		assert(ind >= 0);
	}
	void Apply(Element& element) override
	{
		element.Darken(DarkeningPercentage);
	};
};

class Destroy :public Action {
public:
	Destroy(int ind)
		:Action(ind)
	{

	}
	void Apply(Element& element) override
	{
		element.Create(Type::Empty);
	};
};


class Explosion {
public:
	Explosion(Simulation& sim)
		:world(sim)
	{
	}
	Explosion& operator=(const Explosion& explosion)
	{
		world = explosion.world;
	}
	void ExplodeZone(Vec2I center,  ParticleEffect& list , float ExplosionRadius ,float DarkeningRadius)
	{
		center = world.ScreenToMatrixPos(center);

		int Radius = ExplosionRadius + DarkeningRadius;
		RectI Zone = RectI(Radius * 2, Radius * 2,
			Vec2I(center.x - Radius, center.y - Radius));

		if (Zone.left < 0)
		{
			Zone.width -= std::abs(Zone.left);
			Zone.left = 0;
		}
		else if (Zone.right() > Graphics::ScreenWidth)
		{
			Zone.width -= (Zone.right() - Graphics::ScreenWidth);
		}

		if (Zone.bottom < 0)
		{
			Zone.height -= std::abs(Zone.bottom);
			Zone.bottom = 0;
		}
		else if (Zone.top() > Graphics::ScreenHeight)
		{
			Zone.height -= (Zone.top() - Graphics::ScreenHeight);
		}


		//explosion buffer

	
		std::vector<Action*> eBuffer(Zone.width * Zone.height, nullptr);

		auto dim = world.GetSandboxDim();


		Vec2I pos;
		//top segment
		for (int x = Zone.left; x < Zone.right(); x++)
		{
			pos = Vec2I(x, Zone.bottom);
			float sin = center.GetSin(pos);
			float cos = center.GetCos(pos);

			int dist = 0;
			for (dist = 0; dist < ExplosionRadius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				unsigned int index = matrix_pos.y * dim.width + matrix_pos.x;


				if (index > 0 && index < dim.GetArea())
					if (world.GetElem(index)->GetState() != State::Solid)
					{
						int power = ExplosionRadius - dist + PowerIncrease;
						float VerticalPower =   sin * power * (3.0f / power_rng.GetVal());
						float HorizontalPower = cos * power * (3.0f / power_rng.GetVal());

						unsigned int eIndex = (matrix_pos.y - Zone.bottom) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = new Explode(index, Vec2D(HorizontalPower , VerticalPower ) , world , list);
					}
					else
						break;
			}
			for (; dist <= Radius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				unsigned int index = matrix_pos.y * dim.width + matrix_pos.x;

				if (index > 0 && index < dim.GetArea())
				{
					if (Chance.GetVal() <= 80)
					{
						unsigned int eIndex = (matrix_pos.y - Zone.bottom) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = new Darken(index);
					}
					else
						break;
				}
			}
		}

		//bottom segment
		for (int x = Zone.left; x < Zone.right(); x++)
		{
			pos = Vec2I(x, Zone.top());
			float sin = center.GetSin(pos);
			float cos = center.GetCos(pos);

			int dist = 0;
			for (dist = 0; dist < ExplosionRadius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				unsigned int index = matrix_pos.y * dim.width + matrix_pos.x;


				if (index > 0 && index < dim.GetArea())
					if (world.GetElem(index)->GetState() != State::Solid)
					{
						int power = ExplosionRadius - dist + PowerIncrease;
						float VerticalPower =  sin * power * (3.0f / power_rng.GetVal());
						float HorizontalPower = cos * power * (3.0f / power_rng.GetVal());

						unsigned int eIndex = (matrix_pos.y - Zone.bottom) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = new Explode(index, Vec2D(HorizontalPower, VerticalPower), world , list);
					}
					else
						break;
			}
			for (; dist <= Radius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				int index = matrix_pos.y * dim.width + matrix_pos.x;

				if (index > 0 && index < dim.GetArea())
				{
					if (Chance.GetVal() <= 80)
					{
						unsigned int eIndex = (matrix_pos.y - Zone.bottom) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = new Darken(index);
					}
					else
						break;
				}
			}
		}

		//right segment
		for (int y = Zone.bottom + 1; y < (Zone.top() - 1); y++)
		{
			pos = Vec2I(Zone.right(), y);
			float sin = center.GetSin(pos);
			float cos = center.GetCos(pos);
		
			int dist = 0;
			for (dist = 0; dist < ExplosionRadius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				int index = matrix_pos.y * dim.width + matrix_pos.x;
		
				if (index > 0 && index < dim.GetArea())
					if (world.GetElem(index)->GetState() != State::Solid)
					{
						int power = ExplosionRadius - dist + PowerIncrease;
						float VerticalPower = sin * power * (3.0f / power_rng.GetVal());
						float HorizontalPower = cos * power * (3.0f / power_rng.GetVal());
		
						unsigned int eIndex = (matrix_pos.y - Zone.bottom) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = new Explode(index, Vec2D(HorizontalPower, VerticalPower), world , list);
					}
					else
						break;
			}
			for (; dist <= Radius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				int index = matrix_pos.y * dim.width + matrix_pos.x;
		
				if (index > 0 && index < dim.GetArea())
				{
					if (Chance.GetVal() <= 80)
					{
						unsigned int eIndex = (matrix_pos.y - Zone.bottom) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = new Darken(index);
					}
					else
						break;
				}
			}
		}

		//left segment
		for (int y = Zone.bottom + 1; y < Zone.top() - 1; y++)
		{
			pos = Vec2I(Zone.left, y);
			float sin = center.GetSin(pos);
			float cos = center.GetCos(pos);

			int dist = 0;
			for (dist = 0; dist < ExplosionRadius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				int index = matrix_pos.y * dim.width + matrix_pos.x;

				if (index > 0 && index < dim.GetArea())
					if (world.GetElem(index)->GetState() != State::Solid)
					{
						int power = ExplosionRadius - dist + PowerIncrease;
						float VerticalPower = sin * power * (3.0f / power_rng.GetVal());
						float HorizontalPower = cos * power * (3.0f / power_rng.GetVal());

						unsigned int eIndex = (matrix_pos.y - Zone.bottom) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = new Explode(index, Vec2D(HorizontalPower, VerticalPower), world , list);
					}
					else
						break;
			}
			for (; dist <= Radius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				int index = matrix_pos.y * dim.width + matrix_pos.x;

				if (index > 0 && index < dim.GetArea())
				{
					if (Chance.GetVal() <= 80)
					{
						unsigned int eIndex = (matrix_pos.y - Zone.bottom) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = new Darken(index);
					}
					else
						break;
				}
			}
		}

		for (auto& elem : eBuffer)
		{	
			if (elem != nullptr)
			{
				assert(elem->index >= 0);
				elem->Apply(*world.GetElem(elem->index));
			}
		}

		for (auto& elem : eBuffer)
		{
			delete elem;
			elem = nullptr;
		}
	};
	static constexpr float PowerIncrease = 3.0f;
private:
	RNG Chance = { 1 , 100 };
	Simulation& world;
	RNG power_rng = { 3 , 4};
};