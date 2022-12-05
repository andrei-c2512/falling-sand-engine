#include "Projectile.h"


Projectile::Projectile(Rect& rect, World& world0 ,  Vec2D& vel0, bool Destroyed0)
	:HitBox(rect),world(world0),
	vel(vel0),
	Destroyed(Destroyed0)
{
	Speed = 1.0f;
}

Projectile::Projectile(Rect& rect , World& world0 , float Speed0)
	:Speed(Speed0), world(world0) , HitBox(rect)
{
	vel = { 0.0f , 0.0f };
	Destroyed = true;
}
void Projectile::InitProj(Rect& rect, Vec2D& vel0, bool Destroyed0)
{
	HitBox = rect; vel = vel0; Destroyed = Destroyed0;
}
void Projectile::InitProj(const Projectile& proj)
{
	HitBox = proj.GethBox();
	Destroyed = proj.IsDestroyed();
	vel = proj.GetVel();
}
void Projectile::Launch(Vec2D& vel0, Vec2D& pos0)
{
	Destroyed = false;
	vel = vel0;
	HitBox.left = pos0.x;
	HitBox.top = pos0.y;
}

void Projectile::Destroy()
{
	Destroyed = true;
	vel = { 0.0f , 0.0f };
	HitBox.left = HitBox.top = NULL;
}

void Projectile::Travel(float time)
{
	if (!Destroyed)
	{

		float addX = 60.0f * vel.x * time * Speed, addY = 60.0f * vel.y * time * Speed;

		float x = HitBox.left + addX;
		float y = HitBox.top + addY;

		Rect rect(HitBox.GetDimensions(), Vec2D(x, y));
		if (Graphics::WithinScreen(rect))
		{
			MoveX(time);
			MoveY(time);
		}
		else
			Destroy();

	}
}


void Projectile::DetectCollision(float time)
{  
	//MoveX(time);
	//MoveY(time);
}
void Projectile::DrawProjectile(Graphics& gfx , Color c) {
	if (Destroyed == false)
		gfx.DrawRect(HitBox, c);
}

Rect Projectile::GethBox() const
{
	return HitBox;
}


bool Projectile::IsDestroyed() const
{
	return Destroyed;
}

Vec2D Projectile::GetVel() const
{
	return vel;
}
void Explosive::Destroy()
{
	Explode = true;
}

void Projectile::MoveX(float time)
{
	RectI ZoneX;

	float AddX = std::abs(vel.x * 60.0f * time * Speed);

	int Size = std::ceil(AddX);
	if (int(AddX) % World::ElemSize != 0)
	{
		Size += World::ElemSize - int(AddX) % World::ElemSize;
		Size = int(Size);
	}

	if (vel.x >= 0.0f)
	{
		ZoneX = RectI(Size, HitBox.height, Vec2I(HitBox.right(), HitBox.top));
	}
	else if (vel.x < 0.0f)
	{
		ZoneX = RectI(Size, HitBox.height, Vec2I(HitBox.left - Size, HitBox.top));
	}

	int StartX, EndX, Add;
	std::function<bool(int, int)> Condition;
	if (vel.x > 0.0f)
	{
		StartX = ZoneX.left;
		EndX = ZoneX.right();
		Add = World::ElemSize;
		Condition = [](int nr1, int nr2)
		{
			return nr1 < nr2;
		};
	}
	else
	{
		StartX = ZoneX.right();
		EndX = ZoneX.left;
		Add = -World::ElemSize;
		Condition = [](int nr1, int nr2)
		{
			return nr1 > nr2;
		};
	}

	auto dim = world.GetSandboxDim();
	{
		std::vector<Element> ElementsX;
		for (int x = StartX; Condition(x, EndX); x += Add)
		{
			bool Move = true;
			for (int y = ZoneX.top; y < ZoneX.bottom(); y += World::ElemSize)
			{

				size_t ind = world.GetElemIndScr(Vec2I(x, y));
				if (ind > 0 && ind < dim.GetArea())
				{
					Element& element = *world.GetElem(ind);

					ElementsX.emplace_back(element);
				}
				else
					Move = false;

			}
			for (size_t ind = 0; ind < ElementsX.size(); ind++)
			{

				if (ElementsX[ind].GetState() == State::Solid)
				{

					Move = false;
					break;

				}
			}


			if (Move)
			{
				float add;
				if (AddX < World::ElemSize)
				{
					add = AddX;
					int sign = std::abs(vel.x) / vel.x;
					HitBox.left += sign * add;
					break;
				}
				else
				{
					add = World::ElemSize;
					AddX -= World::ElemSize;
				}
				int sign = std::abs(vel.x) / vel.x;
				HitBox.left += sign * add;
			}
			else
			{
				Destroy();
				break;
			}
			ElementsX.clear();
		}
	}
}

void Projectile::MoveY(float time)
{
	float AddY;
	AddY = float(std::abs(vel.y * 60.0f * time * Speed));

	int Size = std::ceil(AddY);
	if (int(AddY) % World::ElemSize != 0)
	{
		Size += World::ElemSize - int(AddY) % World::ElemSize;
		Size = int(AddY);
	}
	RectI ZoneY;

	if (vel.y >= 0.0f)
	{
		ZoneY = RectI(HitBox.width, Size, Vec2I(HitBox.left, HitBox.bottom()));
	}
	else if (vel.y < 0.0f)
	{
		ZoneY = RectI(HitBox.width, Size, Vec2I(HitBox.left, HitBox.top - Size));
	}
	auto dim = world.GetSandboxDim();


	int StartY, EndY, Add;

	std::function<bool(int, int)> Condition;
	if (vel.y > 0.0f)
	{
		StartY = ZoneY.top;
		EndY = ZoneY.bottom();
		Add = World::ElemSize;

		Condition = [=](int nr1, int nr2)
		{
			return nr1 < nr2;
		};
	}
	else
	{
		StartY = ZoneY.bottom();
		EndY = ZoneY.top;
		Add = -World::ElemSize;

		Condition = [=](int nr1, int nr2)
		{
			return nr1 > nr2;
		};
	}


	{
		std::vector<Element> ElementsY;
		for (int y = StartY; Condition(y, EndY) == true; y += Add)
		{
			bool Move = true;
			for (int x = ZoneY.left; x < ZoneY.right(); x += World::ElemSize)
			{
				size_t ind = world.GetElemIndScr(Vec2I(x, y));
				if (ind > 0 && ind < dim.GetArea())
				{
					Element& element = *world.GetElem(ind);

					ElementsY.emplace_back(element);
				}
				else
					Move = false;
			}

			for (auto& n : ElementsY)
			{
				if (n.GetState() == State::Solid)
				{
					Move = false;
					break;
				}
			}

			if (Move)
			{
				float add;
				if (AddY < World::ElemSize)
				{
					add = AddY;
					int sign = std::abs(vel.y) / vel.y;
					HitBox.top += sign * add;
					break;
				}
				else
				{
					add = World::ElemSize;
					AddY -= World::ElemSize;
				}
				int sign = std::abs(vel.y) / vel.y;
				HitBox.top += sign * add;
			}
			else
			{
				Destroy();
				break;
			}

			ElementsY.clear();
		}
	}

}

void Explosive::Travel(float time)
{
	if (!Destroyed)
	{

		float addX = 60.0f * vel.x * time * Speed, addY = 60.0f * vel.y * time * Speed;

		float x = HitBox.left + addX;
		float y = HitBox.top + addY;

		Rect rect(HitBox.GetDimensions(), Vec2D(x, y));
		if (Graphics::WithinScreen(rect))
		{
			MoveX(time);
			MoveY(time);
		}
		else
			Projectile::Destroy();

		if (Explode)
		{
			Vec2I pos = Vec2I(HitBox.left + HitBox.width / 2, HitBox.top + HitBox.height / 2);
			explosion.ExplodeZone(pos, effect_list);
			Explode = false;

			Projectile::Destroy();
		}
	}
}