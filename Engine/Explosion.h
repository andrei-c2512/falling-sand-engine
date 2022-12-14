#pragma once
#include "World.h"

struct Explosion_verification {
	Explosion_verification(size_t index0, Action type0)
		:type(type0), index(index0)
	{
	}
	size_t index;
	Action type;
};

class Explosion {
public:
	Explosion(World& world0)
		:world(world0)
	{
	}
	void ExplodeZone(Vec2I center, ParticleEffect& list , float ExplosionRadius ,float DarkeningRadius)
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

		if (Zone.top < 0)
		{
			Zone.height -= std::abs(Zone.top);
			Zone.top = 0;
		}
		else if (Zone.bottom() > Graphics::ScreenHeight)
		{
			Zone.height -= (Zone.bottom() - Graphics::ScreenHeight);
		}


		//explosion buffer
		std::vector<Explosion_verification> eBuffer(Zone.width * Zone.height,
			Explosion_verification(0, Action::None));

		auto dim = world.GetSandboxDim();


		Vec2I pos;
		//top segment
		for (int x = Zone.left; x < Zone.right(); x++)
		{
			pos = Vec2I(x, Zone.top);
			float sin = center.GetSin(pos);
			float cos = center.GetCos(pos);

			size_t dist = 0;
			for (dist = 0; dist < ExplosionRadius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				size_t index = matrix_pos.y * dim.width + matrix_pos.x;

				if (index > 0 && index < dim.GetArea())
					if (world.GetElem(index)->GetState() != State::Solid)
					{
						size_t eIndex = (matrix_pos.y - Zone.top) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = Explosion_verification(index, Action::Explode);
					}
					else
						break;
			}
			for (; dist <= Radius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				size_t index = matrix_pos.y * dim.width + matrix_pos.x;

				if (index > 0 && index < dim.GetArea())
				{
					if (Chance.GetVal() <= 80)
					{
						size_t eIndex = (matrix_pos.y - Zone.top) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = Explosion_verification(index, Action::Darken);
					}
					else
						break;
				}
			}
		}

		//bottom segment
		for (int x = Zone.left; x < Zone.right(); x++)
		{
			pos = Vec2I(x, Zone.bottom());
			float sin = center.GetSin(pos);
			float cos = center.GetCos(pos);

			int dist = 0;
			for (dist = 0; dist <= ExplosionRadius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				size_t index = matrix_pos.y * dim.width + matrix_pos.x;

				if (index > 0 && index < dim.GetArea())
					if (world.GetElem(index)->GetState() != State::Solid)
					{
						size_t eIndex = (matrix_pos.y - Zone.top) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = Explosion_verification(index, Action::Explode);
					}
					else
						break;
			}
			for (; dist <= Radius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				size_t index = matrix_pos.y * dim.width + matrix_pos.x;

				if (index > 0 && index < dim.GetArea())
				{
					if (Chance.GetVal() <= 80)
					{
						size_t eIndex = (matrix_pos.y - Zone.top) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = Explosion_verification(index, Action::Darken);
					}
					else
						break;
				}
			}
		}

		//right segment
		for (int y = Zone.top + 1; y < (Zone.bottom() - 1); y++)
		{
			pos = Vec2I(Zone.right(), y);
			float sin = center.GetSin(pos);
			float cos = center.GetCos(pos);

			int dist = 0;
			for (dist = 0; dist <= ExplosionRadius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				size_t index = matrix_pos.y * dim.width + matrix_pos.x;

				if (index > 0 && index < dim.GetArea())
					if (world.GetElem(index)->GetState() != State::Solid)
					{
						size_t eIndex = (matrix_pos.y - Zone.top) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = Explosion_verification(index, Action::Explode);
					}
					else
						break;
			}
			for (; dist <= Radius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				size_t index = matrix_pos.y * dim.width + matrix_pos.x;

				if (index > 0 && index < dim.GetArea())
				{
					if (Chance.GetVal() <= 80)
					{
						size_t eIndex = (matrix_pos.y - Zone.top) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = Explosion_verification(index, Action::Darken);
					}
					else
						break;
				}
			}
		}

		//left segment
		for (int y = Zone.top + 1; y < Zone.bottom() - 1; y++)
		{
			pos = Vec2I(Zone.left, y);
			float sin = center.GetSin(pos);
			float cos = center.GetCos(pos);

			int dist = 0;
			for (dist = 0; dist <= ExplosionRadius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				size_t index = matrix_pos.y * dim.width + matrix_pos.x;

				if (index > 0 && index < dim.GetArea())
					if (world.GetElem(index)->GetState() != State::Solid)
					{
						size_t eIndex = (matrix_pos.y - Zone.top) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = Explosion_verification(index, Action::Explode);
					}
					else
						break;
			}
			for (; dist <= Radius; dist++)
			{
				Vec2I matrix_pos = Vec2I(center.x + int(dist * cos), center.y + int(dist * sin));
				size_t index = matrix_pos.y * dim.width + matrix_pos.x;

				if (index > 0 && index < dim.GetArea())
				{
					if (Chance.GetVal() <= 80)
					{
						size_t eIndex = (matrix_pos.y - Zone.top) * (Zone.width - 1) + (matrix_pos.x - Zone.left);
						eBuffer[eIndex] = Explosion_verification(index, Action::Darken);
					}
					else
						break;
				}
			}
		}

		for (auto& elem : eBuffer)
		{
			if (elem.type == Action::Explode)
			{
				world.GetElem(elem.index)->Explode(list);
			}
			else if (elem.type == Action::Darken)
			{
				world.GetElem(elem.index)->Darken(70);
			}
		}
	};
private:
	RNG Chance = { 1 , 100 };
	World& world;
};