#include "Entity.hpp"

Entity::Entity(Rect rect, Sprite sprite0, float dmg, int health, float speed)
	:HitBox(rect),  sprite(sprite0), Damage(dmg) , Speed(speed)
{
	hp_bar.show_timer = { 4.0f };
	hp_bar.Health = (health);
	hp_bar.MaxHealth = { health };

	hp_bar.dim = Dimensions<short>(   (int)HitBox.width + 2 * HpBar::offset , (int)HitBox.height / 5 );

}

Entity::Entity(const Entity& entity)
{
	if(entity.pWeapon != nullptr)
		pWeapon = std::make_unique<Weapon>(*entity.pWeapon);

	Damage = entity.Damage;
	HitBox = entity.HitBox;
	Speed = entity.Speed;
	vel = entity.vel;
	hp_bar = entity.hp_bar;
	sprite = entity.sprite;
}
void Entity::Draw(Graphics& gfx , Camera& cam)
{
	//gfx.DrawSprite(HitBox.left, HitBox.top, sprite,
	//	RectI(sprite.GetWidth(), sprite.GetHeight(), Vec2I(0, 0)),
	//	Graphics::GetScreenRect(), Effects::Copy{});

	int space = 10; // the space between the health bar and the entity
	Effects::Copy eCopy{};

	Vec2I pos = cam.Transform(HitBox.GetPos());

	gfx.DrawRect(RectI(HitBox.GetDimensions() , std::move(pos)), Colors::Red, eCopy);

	if (hp_bar.show_timer.IsReady() == false)
	{
		float cnt = hp_bar.show_timer.GetTime();
		float limit = hp_bar.show_timer.GetTimeLimit();

		int x = cam.TransformX(HitBox.left - HpBar::offset);
		int y = cam.TransformY(HitBox.bottom - space);
		int bar_width = hp_bar.HpBarWidth();

		if (cnt < limit / 2)
		{ 
			gfx.DrawRect(RectI(bar_width, hp_bar.dim.height,
				Vec2I(x, y)), Colors::Green , eCopy);

			gfx.DrawRect_Border(RectI(bar_width, hp_bar.dim.height,
				Vec2I(x, y)), Colors::White , eCopy);
		}
		else
		{
			float elapsed_time = (cnt - limit / 2);
			unsigned char transparency = 100 - ((elapsed_time / (limit / 2))) * 100.0f;

			Effects::AlphaBlending effect(transparency);

			gfx.DrawRect(RectI(bar_width, hp_bar.dim.height,
				Vec2I(x, y)), Colors::Green, effect);

			gfx.DrawRect_Border(RectI(bar_width, hp_bar.dim.height,
				Vec2I(x , y)), Colors::White, effect);
		}
	}
}

void Entity::Move(World& world , float time)
{
	if (vel.x > 0.0f)
	{
		MoveRight(world, time);
	}
	else if (vel.x < 0.0f)
	{
		MoveLeft(world, time);
	}
	
	if (vel.y > 0.0f)
	{
		MoveUp(world, time);
	}
	else if (vel.y < 0.0f)
	{
		MoveDown(world, time);
	}

	hp_bar.show_timer.Update(time);
	if (hp_bar.Health <= 0)
	{
		Kill(world);
	}

	vel = { 0.0f , 0.0f };
}

void Entity::MoveX(World& world , float time)
{
	RectI ZoneX;

	float AddX = std::abs(vel.x * 60.0f * time);

	int Size = std::ceil(AddX);
	if (int(AddX) % World::ElemSize != 0)
	{
		Size += World::ElemSize - int(AddX) % World::ElemSize;
		Size = int(Size);
	}

	if (vel.x > 0.0f)
	{
		ZoneX = RectI(Size, int(HitBox.height), Vec2I(int(HitBox.right()), int(HitBox.bottom)));
	}
	else if (vel.x <= 0.0f)
	{
		ZoneX = RectI(Size, int(HitBox.height), Vec2I(int(HitBox.left - Size), int(HitBox.bottom)));
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
		StartX = ZoneX.right() ;
		EndX = ZoneX.left ;
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
			for (int y = ZoneX.bottom; y < ZoneX.top(); y += World::ElemSize)
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
						HitBox.bottom -= ElementsX[ind].GetRect().height * (ElementsX.size() - 1 - ind);
						break;
					}
				}
			}

			//here , we use addX as a form of "how many pixels are there left to process", with each move ,
			//the pixel quantity diminishes 
			if (Move)
			{
				if (AddX == 0.0f)
				{
					break;
				}
				float add = 0.0f;

				{
					//checking if it's the first time that we process a move in this frame
					// if it's true , we'll adjust the object position first so that it fits perfectly in line
					//with the sand simulation
					if (x == StartX)
					{
						if (vel.x < 0)
						{
							if (AddX < World::ElemSize)
							{
								add = AddX;
							}
							else
							{
								//this is the distance from entity to the nearest element behind it in the particle simulation
								add = (HitBox.left - (World::ElemSize * (int(HitBox.left / World::ElemSize))));
							}
							HitBox.left -= add;
							AddX -= add;
						}
						else
						{
							
							if (AddX < World::ElemSize)
							{
								add = AddX;
							}
							else
							{
								//this is the distance from entity to the nearest element in front of it in the particle simulation
								add = World::ElemSize - (HitBox.left - (World::ElemSize * (int(HitBox.left / World::ElemSize))));
							}
							HitBox.left += add;
							AddX -= add;
						}
					}
					else
					{
						int sign = std::abs(vel.x) / vel.x;
						if (AddX < World::ElemSize)
						{
							add = AddX;
							HitBox.left += sign * add;
							break;
						}
						else
						{
							add = World::ElemSize;
							HitBox.left += sign * add;
							AddX -= add;
						}
					}
				}
			}
			else
				break;
			ElementsX.clear();
		}
	}
	//assert(AddX == 0.0f);
	vel.x = 0.0f;
}


void Entity::MoveY(World& world , float time)
{
	float AddY;
	//if (vel.y == 0.0f)
	//{
	//	AddY = Gravity * 60.0f * time;
	//}
	//else
		AddY = std::abs(vel.y * 60.0f * time);

	int Size = std::ceil(AddY);
	if (int(AddY) % World::ElemSize != 0)
	{
		Size += World::ElemSize - int(AddY) % World::ElemSize;
		Size = int(AddY);
	}
	RectI ZoneY , PortrustionZoneY;

	if (vel.y >= 0.0f)
	{
		ZoneY = RectI(HitBox.width, Size, Vec2I(HitBox.left, HitBox.top()));
	}
	else if (vel.y < 0.0f)
	{
		ZoneY = RectI(HitBox.width, Size, Vec2I(HitBox.left, HitBox.bottom - Size));
	}
	auto dim = world.GetSandboxDim();

	//we need to store the Y for later , in case the for loop that processes the distance stops because of an obstacle
	float y = 0;

	int StartY, EndY, Add;

	std::function<bool(int, int)> Condition;
	std::function<float()> GetEdge;
	std::function<void(Element & elem)>CheckPortrusion;
	if (vel.y >= 0.0f)
	{
		StartY = ZoneY.bottom;
		EndY = ZoneY.top();
		Add = World::ElemSize;

		Condition = [=](int nr1, int nr2)
		{
			return nr1 < nr2 && nr1 + World::ElemSize < Graphics::ScreenHeight;
		};
		GetEdge = [=]()
		{
			return HitBox.top();
		};
		CheckPortrusion = [=](Element& elem)
		{
			float dist = elem.GetRect().top() - HitBox.top();
			if (dist < 0.0f)
			{
				HitBox.bottom += dist;
			}
		};
	}
	else
	{
		StartY = ZoneY.top();
		EndY = ZoneY.bottom;
		Add = -World::ElemSize;

		Condition = [=](int nr1, int nr2)
		{
			return nr1 > nr2 && nr1 > 0;
		};
		GetEdge = [=]()
		{
			return HitBox.bottom;
		};
		CheckPortrusion = [=](Element& elem)
		{
			float dist = elem.GetRect().bottom - HitBox.bottom;
			if (dist > 0.0f)
			{
				HitBox.bottom += dist;
			}
		};
	}


	{
		std::vector<Element> ElementsY;
		for (y = StartY;
			Condition(y, EndY) == true
			; y += Add)
		{
			ElementsY.clear();
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

			//here , we use addX as a form of "how many pixels are there left to process", with each move ,
			//the pixel quantity diminishes 
			if (Move)
			{
				if (AddY == 0.0f)
				{
					break;
				}
				float add = 0.0f;

				{
					//checking if it's the first time that we process a move in this frame
					// if it's true , we'll adjust the object position first so that it fits perfectly in line
					//with the sand simulation
					if (y == StartY)
					{
						if (vel.y < 0)
						{
							if (AddY < World::ElemSize)
							{
								add = AddY;
							}
							else
							{
								//this is the distance from entity to the nearest element behind it in the particle simulation
								add = (HitBox.bottom - (World::ElemSize * (int(HitBox.bottom / World::ElemSize))));
							}
							HitBox.bottom -= add;
							AddY -= add;
						}
						else
						{

							if (AddY < World::ElemSize)
							{
								add = AddY;
							}
							else
							{
								//this is the distance from entity to the nearest element in front of it in the particle simulation
								add = World::ElemSize - (HitBox.bottom - (World::ElemSize * (int(HitBox.bottom / World::ElemSize))));
							}
							HitBox.bottom += add;
							AddY -= add;
						}
					}
					else
					{
						int sign = std::abs(vel.y) / vel.y;
						if (AddY < World::ElemSize)
						{
							add = AddY;
							HitBox.bottom += sign * add;
							break;
						}
						else
						{
							add = World::ElemSize;
							HitBox.bottom += sign * add;
							AddY -= add;
						}
					}
				}

			}
			else
			{
				break;
			}
		}
		//CheckProtrusionY(world, y);

		ElementsY.clear();
	}
	//assert(AddY == 0.0f);
	vel.y = 0.0f;

}

bool Entity::CheckFullCollision(Entity& entity)
{
	return CheckWeaponCollision(entity) && CheckBodyCollision(entity);
}
bool Entity::CheckWeaponCollision(Entity& entity)
{
	return pWeapon->Collision(entity.GetRect());
}
bool Entity::CheckBodyCollision(Entity& entity)
{
	return entity.HitBox.Collision(HitBox);
}

bool Entity::CheckCollision(Entity& entity)
{
	return CheckBodyCollision(entity);
}
void Entity::SetPos(Vec2I& pos)
{
	HitBox.SetPos(Vec2D(pos.x , pos.y));
}

Rect Entity::GetRect() const
{
	return HitBox;
}

void Entity::DamageEntity(int damage)
{
	hp_bar.Health -= damage;
	hp_bar.show_timer.ResetTimer();
}
void Entity::MoveRight(World& world, float time)
{
	float AddX = vel.x * time * 60.0f;

	//basically i am getting the modulus for a float 
	float offset = HitBox.left - (int(HitBox.left) / World::ElemSize) * World::ElemSize;
	int LineX = HitBox.right();
	if (offset != 0.0f)
	{
		LineX += World::ElemSize;
	}

	while (AddX)
	{
		bool Move = true;
		if (LineX > Graphics::WorldArea.right())
		{
			break;
		}
		std::vector<State> elem_list;
		for (int y = HitBox.bottom; y < HitBox.top(); y += World::ElemSize)
		{
			Vec2I pos = world.ScreenToMatrixPos(Vec2I(LineX, y));
			int index = pos.y * World::SandboxDim.width + pos.x;
			elem_list.emplace_back(world.GetElem(index)->GetState());
		}


		for(int ind = 0 ; ind < elem_list.size() ; ind++)
		{
			if (elem_list[ind] == State::Solid)
			{
				if (ind >= elem_list.size() - 5)
				{
					HitBox.bottom -= (ind - (elem_list.size() - 5)) * World::ElemSize;
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
				if (AddX <= dist)
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

void Entity::MoveLeft(World& world, float time)
{
	float AddX = std::abs(vel.x * time * 60.0f);
	int LineX = HitBox.left - World::ElemSize;
	while (AddX)
	{
		if (LineX < Graphics::WorldArea.left)
		{
			break;
		}
		bool Move = true;
		std::vector<State> elem_list;
		for (int y = HitBox.bottom; y < HitBox.top(); y += World::ElemSize)
		{
			Vec2I pos = world.ScreenToMatrixPos(Vec2I(LineX, y));
			int index = pos.y * World::SandboxDim.width + pos.x;
			elem_list.emplace_back(world.GetElem(index)->GetState());
		}

		for (int ind = 0; ind < elem_list.size(); ind++)
		{
			if (elem_list[ind] == State::Solid)
			{
				if (ind > elem_list.size() - 5)
				{
					HitBox.bottom -= (ind - (elem_list.size() - 5))  * World::ElemSize;
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
				break;
			}
		}

		LineX -= World::ElemSize;
	}
}

void Entity::MoveUp(World& world, float time)
{
	float AddY = vel.y * time * 60.0f;

	//basically i am getting the modulus for a float 
	float offset = HitBox.bottom - (int(HitBox.bottom) / World::ElemSize) * World::ElemSize;

	int LineY = HitBox.top();
	if (offset != 0.0f)
	{
		LineY += World::ElemSize;
	}

	while (AddY)
	{
		if (LineY > Graphics::WorldArea.top())
		{
			break;
		}
		bool Move = true;
		std::vector<State> elem_list;
		for (int x = HitBox.left; x < HitBox.right(); x += World::ElemSize)
		{
			Vec2I pos = world.ScreenToMatrixPos(Vec2I(x, LineY));
			int index = pos.y * World::SandboxDim.width + pos.x;
			elem_list.emplace_back(world.GetElem(index)->GetState());
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
			//distance to the next cell
			float dist = World::ElemSize - offset;

			if (offset != 0.0f)
			{
				if (AddY <= dist)
				{
					HitBox.bottom += AddY;
					break;
				}
				else
				{
					HitBox.bottom += dist;
					break;
				}
			}
			else
				break;
		}

		LineY += World::ElemSize;
	}
}

void Entity::MoveDown(World& world, float time)
{
	float AddY = std::abs(vel.y * time * 60.0f);
	int LineY = HitBox.bottom - World::ElemSize;
	while (AddY)
	{
		bool Move = true;
		if (LineY < Graphics::WorldArea.bottom)
		{
			break;
		}
		std::vector<State> elem_list;
		for (int x = HitBox.left; x < HitBox.right(); x += World::ElemSize)
		{
			Vec2I pos = world.ScreenToMatrixPos(Vec2I(x, LineY));
			int index = pos.y * World::SandboxDim.width + pos.x;
			elem_list.emplace_back(world.GetElem(index)->GetState());
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
				break;
			}
		}

		LineY -= World::ElemSize;
	}
}

void Entity::Kill(World& world)
{
	Dead = true;
	for (int y = HitBox.bottom; y < HitBox.top(); y++)
	{
		for (int x = HitBox.left; x < HitBox.right(); x++)
		{
			Vec2I pos = world.ScreenToMatrixPos(Vec2I(x, y));
			int index = pos.y * World::SandboxDim.width + pos.x;
	
			world.AddToSpawnList(World::Spawn(std::move(index), Type::Sand));
		}
	}
}