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


	RectI Head = RectI(sHead.GetWidth(), sHead.GetHeight(), Vec2I(HitBox.pos()));

	auto pos = Head.GetCenter();
	auto MousePos = mouse.GetPos();
	gfx.DrawAngledSprite(HitBox.left , HitBox.top, sHead, RectI(sHead.GetWidth(),
		sHead.GetHeight(), Vec2I{ 0 , 0 }) , MousePos);

	gfx.DrawRectI_Border(HitBox, Colors::Cyan);
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

void Player::MoveX(float time )
{
	RectI ZoneX = { 0 , 0 , Vec2I{0 , 0} };

	int AddX = vel.x * 60.0f * time;
	if (int(AddX) % World::ElemSize > World::ElemSize / 2)
	{
		AddX += World::ElemSize - int(AddX) % World::ElemSize;
		AddX = int(AddX);
	}

	if (AddX > 0.0f)
	{
		ZoneX = RectI(AddX, HitBox.height, Vec2I(HitBox.right(), HitBox.top));
	}
	else if (AddX < 0.0f)
	{
		ZoneX = RectI(std::abs(AddX), HitBox.height, Vec2I(HitBox.left, HitBox.top));
	}

	auto dim = world.GetSandboxDim();
	{
		std::vector<Element> ElementsX;
		for (int x = ZoneX.left; x < ZoneX.right(); x += World::ElemSize)
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
				int sign = std::abs(AddX) / AddX;
				HitBox.left += sign * World::ElemSize;
			}
			ElementsX.clear();
		}
	}

	vel.x = 0.0f;
}

void Player::MoveY(float time)
{
	int AddY = vel.y * 60.0f * time;
	if (vel.y == 0.0f)
	{
		AddY = Gravity;
	}

	if (int(AddY) % World::ElemSize > World::ElemSize / 2)
	{
		AddY = World::ElemSize - int(AddY) % World::ElemSize;
		AddY = int(AddY);
	}
	RectI ZoneY = { 0 , 0 , Vec2I{0 , 0} };

	if (AddY > 0.0f)
	{
		ZoneY = RectI(HitBox.width, AddY, Vec2I(HitBox.left, HitBox.bottom()));
	}
	else if (AddY < 0.0f)
	{
		ZoneY = RectI(HitBox.width, std::abs(AddY), Vec2I(HitBox.left, HitBox.top + std::abs(AddY)));
	}
	auto dim = world.GetSandboxDim();

	{
		std::vector<Element> ElementsY;
		for (int y = ZoneY.top; y < ZoneY.bottom(); y += World::ElemSize)
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
				int sign = std::abs(AddY) / AddY;
				HitBox.top += sign * World::ElemSize;
			}

			ElementsY.clear();
		}
	}

	vel.y = 0.0f;
}