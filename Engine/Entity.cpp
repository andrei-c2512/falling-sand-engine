#include "Entity.h"

Entity::Entity(Rect rect, Sprite sprite0, float dmg, int health, float speed)
	:HitBox(rect),  sprite(sprite0), Damage(dmg) , Health(health) , Speed(speed)
{
}

void Entity::Draw(Graphics& gfx)
{
	gfx.DrawSprite(HitBox.left, HitBox.top, sprite,
		RectI(sprite.GetWidth(), sprite.GetHeight(), Vec2I(0, 0)),
		Graphics::GetScreenRect(), Effects::Copy{});
}

void Entity::Move(World& world ,float time)
{
	MoveX(world , time);
	MoveY(world , time);
}

void Entity::MoveX(World& world ,float time)
{
	RectI ZoneX;

	float AddX = std::abs(vel.x * 60.0f * time);

	int Size = AddX;
	if (int(AddX) % World::ElemSize != 0)
	{
		Size += World::ElemSize - int(AddX) % World::ElemSize;
		Size = int(Size);
	}

	if (vel.x > 0.0f)
	{
		ZoneX = RectI(Size, int(HitBox.height), Vec2I(int(HitBox.right()), int(HitBox.top)));
	}
	else if (vel.x <= 0.0f)
	{
		ZoneX = RectI(Size, int(HitBox.height), Vec2I(int(HitBox.left - Size), int(HitBox.top)));
	}

	int StartX, EndX, Add;
	std::function<bool(int, int)> Condition;
	std::function<float(Rect, RectI)> dist;
	if (vel.x > 0.0f)
	{
		StartX = ZoneX.left;
		EndX = ZoneX.right();
		Add = World::ElemSize;
		Condition = [](int nr1, int nr2)
		{
			return nr1 < (nr2);
		};
		dist = [](Rect rect, RectI elem)
		{
			return std::abs(rect.right() - elem.left);
		};
	}
	else
	{
		StartX = ZoneX.right();
		EndX = ZoneX.left;
		Add = -World::ElemSize;
		Condition = [](int nr1, int nr2)
		{
			return nr1 > (nr2);
		};
		dist = [](Rect rect, RectI elem)
		{
			return std::abs(elem.right() - rect.left);
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
					if (ind < ElementsX.size() - 4)
					{
						{
							Move = false;
							break;
						}
					}
					else
					{
						HitBox.top -= ElementsX[ind].GetRect().height * (ElementsX.size() - 1 - ind);
						break;
					}
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
				break;
			ElementsX.clear();
		}
	}
	//assert(AddX == 0.0f);
	vel.x = 0.0f;
}

void Entity::MoveY(World& world ,float time)
{
	float AddY;
	if (vel.y == 0.0f)
	{
		AddY = Gravity * 60.0f * time;
	}
	else
		AddY = std::abs(vel.y * 60.0f * time);

	int Size = AddY;
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
	if (vel.y >= 0.0f)
	{
		StartY = ZoneY.top;
		EndY = ZoneY.bottom();
		Add = World::ElemSize;

		Condition = [=](int nr1, int nr2)
		{
			return nr1 < nr2&& nr1 + World::ElemSize < Graphics::ScreenHeight;
		};
	}
	else
	{
		StartY = ZoneY.bottom();
		EndY = ZoneY.top;
		Add = -World::ElemSize;

		Condition = [=](int nr1, int nr2)
		{
			return nr1 > nr2 && nr1 > 0;
		};
	}


	{
		std::vector<Element> ElementsY;
		for (int y = StartY;
			Condition(y, EndY) == true
			; y += Add)
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
				{
					Move = false;
					break;
				}
			}
			if (ElementsY.size() == 0)
			{
				break;
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
				short sign = 1;


				if (y == StartY)
				{
					add = World::ElemSize;
					if (vel.y < 0)
					{
						add = (HitBox.left - (World::ElemSize * (int(HitBox.left / World::ElemSize))));
					}
					else
					{
						add -= (HitBox.left - (World::ElemSize * (int(HitBox.left / World::ElemSize))));
					}
				}
				else if (AddY < World::ElemSize)
				{
					if (vel.y != 0.0f)
					{
						sign = short(std::abs(vel.y) / vel.y);
					}
					add = AddY;
					AddY -= add;
					HitBox.top += sign * add;
					break;
				}
				else
				{
					if (vel.y != 0.0f)
					{
						sign = short(std::abs(vel.y) / vel.y);
					}
					add = World::ElemSize;
					AddY -= World::ElemSize;
				}
				HitBox.top += sign * add;
			}
			else
				break;

			ElementsY.clear();
		}
	}
	//assert(AddY == 0.0f);
	vel.y = 0.0f;

}


void Entity::CheckCollisions(Player& player)
{
	const auto& list = player.GetProj_list();

	for (auto& n : list)
	{
		if (n->GethBox().Collision(HitBox))
		{
			n->Destroy();
		}
	}
}

void Entity::DetermineMovement(Player& player)
{
	Rect pRect = player.GetRect();

	Vec2D pCenter = pRect.GetCenter(); // player center
	Vec2D eCenter = HitBox.GetCenter(); // enemy center

}