#include "Projectile.h"


Projectile::Projectile(const Rect& rect, Simulation& world0 , const Vec2D& vel0, bool Destroyed0)
	:HitBox(rect),simulation(world0),
	vel(vel0),
	Destroyed(Destroyed0)
{
	Speed = 1.0f;
}

Projectile::Projectile(const Rect& rect , Simulation& world0 , float Speed0)
	:Speed(Speed0), simulation(world0) , HitBox(rect)
{
	vel = { 0.0f , 0.0f };
	Destroyed = true;
}
void Projectile::InitProj(const Rect& rect, const Vec2D& vel0, bool Destroyed0)
{
	HitBox = rect; vel = vel0; Destroyed = Destroyed0;
}
void Projectile::InitProj(const Projectile& proj)
{
	HitBox = proj.GethBox();
	Destroyed = proj.IsDestroyed();
	vel = proj.GetVel();
}
void Projectile::Launch(const Vec2D& vel0, const Vec2D& pos0)
{
	Destroyed = false;
	vel = vel0;
	HitBox.left = pos0.x;
	HitBox.bottom = pos0.y;
}

void Projectile::Destroy()
{
	Destroyed = true;
	vel = { 0.0f , 0.0f };
	HitBox.left = HitBox.bottom = NULL;
}

void Projectile::Travel(float time)
{
	if (!Destroyed)
	{

		float addX = 60.0f * vel.x * time * Speed, addY = 60.0f * vel.y * time * Speed;

		float x = HitBox.left + addX;
		float y = HitBox.bottom + addY;

		Rect rect(HitBox.GetDimensions(), Vec2D(x, y));
		if (Graphics::WithinScreen(rect))
		{
			if (vel.x > 0.0f)
			{
				MoveRight(time);
			}
			else
			{
				MoveLeft(time);
			}
			if (!Destroyed)
			{
				if (vel.y > 0.0f)
				{
					MoveDown(time);
				}
				else
				{
					MoveUp(time);
				}
			}
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
void Projectile::DrawProjectile(Graphics& gfx ,Camera& ct , Color c) {
	if (Destroyed == false)
		gfx.DrawRect(RectI(HitBox.GetDimensions() ,ct.Transform(HitBox.GetPos())), c, Effects::Copy{});
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
	Vec2I pos = Vec2I(HitBox.left + HitBox.width / 2, HitBox.bottom + HitBox.height / 2);
	explosion.ExplodeZone(pos, effect_list , ExplosionRadius , DarkeningRadius);

	Projectile::Destroy();
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
		ZoneX = RectI(Size, HitBox.height, Vec2I(HitBox.right(), HitBox.bottom));
	}
	else if (vel.x < 0.0f)
	{
		ZoneX = RectI(Size, HitBox.height, Vec2I(HitBox.left - Size, HitBox.bottom));
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
			return nr1 < (nr2 - World::ElemSize);
		};
	}
	else
	{
		StartX = ZoneX.right();
		EndX = ZoneX.left;
		Add = -World::ElemSize;
		Condition = [](int nr1, int nr2)
		{
			return nr1 > (nr2 + World::ElemSize);
		};
	}

	auto dim = simulation.GetSandboxDim();
	{
		std::vector<Element> ElementsX;
		for (int x = StartX; Condition(x, EndX); x += Add)
		{
			bool Move = true;
			for (int y = ZoneX.bottom; y < ZoneX.top(); y += World::ElemSize)
			{

				size_t ind = simulation.GetElemIndScr(Vec2I(x, y));
				if (ind > 0 && ind < dim.GetArea())
				{
					Element& element = *simulation.GetElem(ind);

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
				if (x == StartX)
				{
					add = World::ElemSize;
					if (vel.x < 0)
					{
						add = (HitBox.left - (World::ElemSize * (int(HitBox.left / World::ElemSize))));
					}
					else
					{
						add -= (HitBox.left - (World::ElemSize * (int(HitBox.left / World::ElemSize))));
					}
				}
				else if (AddX < World::ElemSize)
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



void Explosive::Travel(float time)
{
	if (!Destroyed)
	{

		float addX = 60.0f * vel.x * time * Speed, addY = 60.0f * vel.y * time * Speed;

		float x = HitBox.left + addX;
		float y = HitBox.bottom + addY;

		Rect rect(HitBox.GetDimensions(), Vec2D(x, y));
		if (Graphics::WithinScreen(rect))
		{
			if (vel.x > 0.0f)
			{
				MoveRight(time);
			}
			else
			{
				MoveLeft(time);
			}
			if (!Destroyed)
			{
				if (vel.y > 0.0f)
				{
					MoveDown(time);
				}
				else
				{
					MoveUp(time);
				}
			}
		}
		else
			Projectile::Destroy();

	}
}

float Projectile::GetDamage() const {
	return dmg_rand.GetVal() + BaseDamage * (Speed / 2);
}

void Projectile::MoveRight( float time)
{
	float AddX = vel.x * time * 60.0f;
	float offset = HitBox.left - (int(HitBox.left) / World::ElemSize) * World::ElemSize;
	int LineX = HitBox.right();
	if (offset != 0.0f)
	{
		LineX += World::ElemSize;
	}
	while (AddX)
	{
		if (LineX >= Graphics::ScreenWidth)
		{
			break;
		}

		bool Move = true;
		std::vector<State> elem_list;
		for (int y = HitBox.bottom; y < HitBox.top(); y += World::ElemSize)
		{
			Vec2I pos = simulation.ScreenToMatrixPos(Vec2I(LineX, y));
			int index = pos.y * World::SandboxDim.width + pos.x;
			elem_list.emplace_back(simulation.GetElem(index)->GetState());
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
			//basically i am getting the modulus for a float 
			float offset = HitBox.left - (int(HitBox.left) / World::ElemSize) * World::ElemSize;
			//distance to the next cell
			float dist = World::ElemSize - offset;

			if (AddX <= dist)
			{
				HitBox.left += AddX;
				break;
			}
			else
			{
				HitBox.left += dist;
				Destroy();
				break;
			}
		}

		LineX += World::ElemSize;
	}
}

void Projectile::MoveLeft(float time)
{
	float AddX = std::abs(vel.x * time * 60.0f);
	int LineX = HitBox.left - World::ElemSize;
	while (AddX)
	{
		if (LineX < 0)
		{
			break;
		}
		bool Move = true;
		std::vector<State> elem_list;
		for (int y = HitBox.bottom; y < HitBox.top(); y += World::ElemSize)
		{
			Vec2I pos = simulation.ScreenToMatrixPos(Vec2I(LineX, y));
			int index = pos.y * World::SandboxDim.width + pos.x;
			elem_list.emplace_back(simulation.GetElem(index)->GetState());
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
			if (AddX < World::ElemSize)
			{
				HitBox.left -= AddX;
				AddX = 0;
			}
			else
			{
				HitBox.left -= World::ElemSize;
				AddX -= World::ElemSize;
			}
		}
		else
		{
			//basically i am getting the modulus for a float 
			//distance to the next cell
			float dist = HitBox.left - (int(HitBox.left) / World::ElemSize) * World::ElemSize;

			if (AddX <= dist)
			{
				HitBox.left -= AddX;
				break;
			}
			else
			{
				HitBox.left -= dist;
				Destroy();
				break;
			}
		}

		LineX -= World::ElemSize;
	}
}

void Projectile::MoveDown(float time)
{
	float AddY = vel.y * time * 60.0f;
	float offset = HitBox.bottom - (int(HitBox.bottom) / World::ElemSize) * World::ElemSize;

	int LineY = HitBox.top();
	if (offset != 0.0f)
	{
		LineY += World::ElemSize;
	}
	while (AddY)
	{
		if (LineY >= Graphics::ScreenHeight)
		{
			break;
		}

		bool Move = true;
		std::vector<State> elem_list;
		for (int x = HitBox.left; x < HitBox.right(); x += World::ElemSize)
		{
			Vec2I pos = simulation.ScreenToMatrixPos(Vec2I(x, LineY));
			int index = pos.y * World::SandboxDim.width + pos.x;
			elem_list.emplace_back(simulation.GetElem(index)->GetState());
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
				HitBox.bottom += AddY;
				AddY = 0;
			}
			else
			{
				HitBox.bottom += World::ElemSize;
				AddY -= World::ElemSize;
			}
		}
		else
		{
			//basically i am getting the modulus for a float 
			float offset = HitBox.bottom - (int(HitBox.bottom) / World::ElemSize) * World::ElemSize;
			//distance to the next cell
			float dist = World::ElemSize - offset;

			if (AddY <= dist)
			{
				HitBox.bottom += AddY;
				break;
			}
			else
			{
				HitBox.bottom += dist;
				Destroy();
				break;
			}
		}

		LineY += World::ElemSize;
	}
}

void Projectile::MoveUp(float time)
{
	float AddY = std::abs(vel.y * time * 60.0f);
	int LineY = HitBox.bottom - World::ElemSize;
	while (AddY)
	{
		if (LineY < 0)
		{
			break;
		}
		bool Move = true;
		std::vector<State> elem_list;
		for (int x = HitBox.left; x < HitBox.right(); x += World::ElemSize)
		{
			Vec2I pos = simulation.ScreenToMatrixPos(Vec2I(x, LineY));
			int index = pos.y * World::SandboxDim.width + pos.x;
			elem_list.emplace_back(simulation.GetElem(index)->GetState());
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
				HitBox.bottom -= AddY;
				AddY = 0;
			}
			else
			{
				HitBox.bottom -= World::ElemSize;
				AddY -= World::ElemSize;
			}
		}
		else
		{
			//basically i am getting the modulus for a float 
			//distance to the next cell
			float dist = HitBox.bottom - (int(HitBox.bottom) / World::ElemSize) * World::ElemSize;

			if (AddY <= dist)
			{
				HitBox.bottom -= AddY;
				break;
			}
			else
			{
				HitBox.bottom -= dist;
				Destroy();
				break;
			}
		}

		LineY -= World::ElemSize;
	}
}