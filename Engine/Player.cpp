#include "Player.h"
#include "Effects.h"
#include <cmath>
Player::Player(Mouse& mouse0 , Sprite& bodysprite, Sprite& headsprite,  float speed0)
	: sBody(bodysprite) , sHead(headsprite), Speed(speed0), mouse(mouse0)
{
	sBody.ScaleSprite(World::ElemSize);
	sHead.ScaleSprite(World::ElemSize);
	HitBox.left = HitBox.top = 400;
	HitBox.width = sHead.GetWidth();
	HitBox.height = sHead.GetHeight() + sBody.GetHeight();

	HitBox.left = sBody.GetWidth() / 2;
	HitBox.top = sBody.GetHeight() / 2;

	vel = { 0.0f , 0.0f }; 

	Speed = 20.0f;
}

void Player::Draw(Graphics& gfx)
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

	gfx.DrawRect_Border(HitBox, Colors::Green, Effects::Copy{});

	pWeapon->Draw(gfx);
}

void Player::UpdateMovement(World& world , Keyboard& kbd , float time)
{
	{
		if (kbd.KeyIsPressed('W'))
		{
			vel.y = -Speed;
		}
		if (kbd.KeyIsPressed('S'))
		{
			vel.y = +Speed;
		}
		//MoveY(world, time);

		if (kbd.KeyIsPressed('A'))
		{
			vel.x = -Speed;
			//MoveX(world, time);
		}
		if (kbd.KeyIsPressed('D'))
		{
			vel.x = +Speed;
		//	MoveX(world, time);
		}
		Entity::Move(world, time);
	}

}



void Player::GiveWeapon(std::unique_ptr<Weapon> wp)
{
	pWeapon = std::move(wp);
}

void Player::UseWeapon( float time)
{
	if (mouse.LeftIsPressed())
	{
		Vec2D mouse_pos = Vec2D(mouse.GetPosX() , mouse.GetPosY());
		pWeapon->Use(mouse_pos);
	}
	pWeapon->Update(time);
}

const std::forward_list<Rect> Player::GetProj_list()
{
	return pWeapon->GetWeaponHitBoxes();
}

bool Player::CheckCollision(Entity& entity)
{
	return CheckWeaponCollision(entity);
}

bool Player::CheckCollision(NPC& npc)
{
	if (pWeapon->Collision(npc.GetRect()))
	{
		npc.ChangeTarget(this);
		npc.DamageEntity(pWeapon->GetDamage());
		return true;
	}
	return false;
}