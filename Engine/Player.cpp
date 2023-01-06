#include "Player.h"
#include "Effects.h"
#include <cmath>
Player::Player(Mouse& mouse0 , Sprite& bodysprite, Sprite& headsprite,  Camera& camera0 ,float speed0)
	: sBody(bodysprite) , sHead(headsprite), Speed(speed0), mouse(mouse0), camera(camera0)
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

void Player::Draw(Graphics& gfx , Camera& cam)
{
	Effects::Copy e;
	int x = HitBox.left - (std::abs(sHead.GetWidth() - sBody.GetWidth()) / 2);
	Vec2I pos_body = cam.Transform(Vec2D(x, HitBox.top + sHead.GetHeight()));

	gfx.DrawSprite(std::move(pos_body), sBody, RectI(sBody.GetWidth(),
		sBody.GetHeight(), Vec2I{ 0 , 0 }), Graphics::GetScreenRect() , e);
	
	
	Rect Head = Rect(sHead.GetWidth(), sHead.GetHeight(), Vec2D(HitBox.pos()));
	
	auto pos = Head.GetCenter();
	auto MousePos = mouse.GetPos();
	gfx.DrawAngledSprite(cam.Transform(HitBox.GetPos()), sHead, RectI(sHead.GetWidth(),
		sHead.GetHeight(), Vec2I{ 0 , 0 }) , cam.Transform(MousePos));
	
	gfx.DrawRect_Border(RectI(HitBox.GetDimensions(), cam.Transform(HitBox.GetPos())), Colors::Green, Effects::Copy{});
	
	pWeapon->Draw(gfx, cam);
}

void Player::UpdateMovement(World& world , Keyboard& kbd , float time)
{
	{
		if (kbd.KeyIsPressed('W'))
		{
			vel.y = +Speed;
		}
		if (kbd.KeyIsPressed('S'))
		{
			vel.y = -Speed;
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
		Vec2I new_pos = Vec2I(HitBox.left - Graphics::ScreenWidth / 2, HitBox.top - Graphics::ScreenHeight / 2);
		camera.SetPos(new_pos);
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