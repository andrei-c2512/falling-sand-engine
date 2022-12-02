#include "Player.h"
#include "Effects.h"
#include <cmath>
Player::Player(Sprite& bodysprite, Sprite& headsprite, World& world0 , float speed0)
	: sBody(bodysprite) , sHead(headsprite), Speed(speed0), world(world0)
{
	sBody.ScaleSprite(World::ElemSize);
	sHead.ScaleSprite(World::ElemSize);
	HitBox.left = HitBox.top = 400;
	HitBox.width = sHead.GetWidth();
	HitBox.height = sHead.GetHeight() + sBody.GetHeight();

	HitBox.left = sBody.GetWidth() / 2;
	HitBox.top = sBody.GetHeight() / 2;

	vel = { 0.0f , 0.0f };
}

void Player::Draw(Graphics& gfx , Mouse& mouse)
{
	Effects::Copy e;
	int x = HitBox.left - (std::abs(sHead.GetWidth() - sBody.GetWidth()) / 2);
	gfx.DrawSprite(x, HitBox.top + sHead.GetHeight(), sBody, RectI(sBody.GetWidth(),
		sBody.GetHeight(), Vec2I{ 0 , 0 }), Graphics::GetScreenRect() , e);


	Rect Head = Rect(sHead.GetWidth(), sHead.GetHeight(), Vec2D(HitBox.pos()));

	auto pos = Head.GetCenter();
	auto MousePos = mouse.GetPos();
	gfx.DrawAngledSprite(HitBox.left , HitBox.top, sHead, RectI(sHead.GetWidth(),
		sHead.GetHeight(), Vec2I{ 0 , 0 }) , MousePos);

	gfx.DrawRect_Border(HitBox, Colors::Cyan);

	pWeapon->DrawProjectiles(gfx, Colors::Red);
}

void Player::UpdateMovement(Keyboard& kbd , float time)
{
	if (kbd.KeyIsPressed('W'))
	{
		vel.y = -Speed;
	}
	if (kbd.KeyIsPressed('S'))
	{
		vel.y = +Speed;
	}
	MoveY(time);

	if (kbd.KeyIsPressed('A'))
	{
		vel.x = -Speed;
		MoveX(time);
	}
	if (kbd.KeyIsPressed('D'))
	{
		vel.x = +Speed;
		MoveX(time);
	}
}




void Player::Move(Keyboard& kbd , float time)
{
	
}


void Player::MoveX(float time)
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
		ZoneX = RectI(Size, HitBox.height, Vec2I(HitBox.right(), HitBox.top));
	}
	else if (vel.x <= 0.0f)
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
		for (int x = StartX; Condition(x , EndX); x += Add)
		{
			bool Move = true;
			for (int y = ZoneX.top; y < ZoneX.bottom(); y += World::ElemSize)
			{

				size_t ind = world.GetElemScr(Vec2I(x, y));
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
						Move = false;
						break;
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
			ElementsX.clear();
		}
	}
	//assert(AddX == 0.0f);
	vel.x = 0.0f;
}

void Player::MoveY(float time)
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
	RectI ZoneY = { 0 , 0 , Vec2I{0 , 0} };

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

	std::function<bool(int , int)> Condition;
	if (vel.y > 0.0f)
	{
		StartY = ZoneY.top;
		EndY = ZoneY.bottom();
		Add = World::ElemSize;

		Condition = [=](int nr1 , int nr2)
		{
			return nr1 < nr2;
		};
	}
	else
	{
		StartY = ZoneY.bottom();
		EndY = ZoneY.top;
		Add = -World::ElemSize;

		Condition = [=](int nr1 , int nr2)
		{
			return nr1 > nr2;
		};
	}


	{
		std::vector<Element> ElementsY;
		for (int y = StartY; Condition(y , EndY) == true; y += Add)
		{
			bool Move = true;
			for (int x = ZoneY.left; x < ZoneY.right(); x += World::ElemSize)
			{
				size_t ind = world.GetElemScr(Vec2I(x, y));
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
				int sign = 1;

				if (vel.y != 0.0f)
				{
					sign = std::abs(vel.y) / vel.y;
				}

				if (AddY < World::ElemSize)
				{
					add = AddY;
					AddY -= add;
					HitBox.top += sign * add;
					break;
				}
				else
				{
					add = World::ElemSize;
					AddY -= World::ElemSize;
				}
				HitBox.top += sign * add;
			}

			ElementsY.clear();
		}
	}
	//assert(AddY == 0.0f);
	vel.y = 0.0f;

}
void Player::GiveWeapon(std::unique_ptr<Weapon> wp)
{
	pWeapon = std::move(wp);
}

void Player::UseWeapon(Mouse& mouse , float time)
{
	pWeapon->Update(mouse, time);
}