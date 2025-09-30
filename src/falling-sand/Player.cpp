#include "falling-sand/Player.hpp"
#include "engine2d/ui/CoordinateShower.hpp"
#include <cmath>

namespace sand {
	Player::Player(chili::Mouse& mouse0, const e2d::Sprite& bodysprite, const e2d::Sprite& headsprite, e2d::Camera& camera0, float speed0)
		: sBody(bodysprite), sHead(headsprite), Speed(speed0), mouse(mouse0), camera(camera0)
	{
		sBody.ScaleSprite(World::ElemSize);
		sHead.ScaleSprite(World::ElemSize);
		HitBox.left = HitBox.bottom = 400;
		HitBox.width = sHead.GetWidth();
		HitBox.height = sHead.GetHeight() + sBody.GetHeight();

		HitBox.left = sBody.GetWidth() / 2;
		HitBox.bottom = sBody.GetHeight() / 2;

		vel = { 0.0f , 0.0f };
	}

	void Player::Draw(e2d::Renderer& gfx, const e2d::Camera& cam)
	{
		int x = HitBox.left - (std::abs(sHead.GetWidth() - sBody.GetWidth()) / 2);

		e2d::Vec2I pos_body = cam.Transform(e2d::Vec2D(x, HitBox.top() - sHead.GetHeight()));

		/*gfx.DrawSprite(std::move(pos_body), sBody, RectI(sBody.GetWidth(),
			sBody.GetHeight(), Vec2I{ 0 , 0 }), Graphics::GetScreenRect() , e);


		auto MousePos = mouse.GetPos();
		gfx.DrawAngledSprite(cam.Transform(Vec2I(HitBox.left , HitBox.top())), sHead, RectI(sHead.GetWidth(),
			sHead.GetHeight(), Vec2I{ 0 , 0 }) , cam.Transform(MousePos));

		gfx.DrawRect_Border(RectI(HitBox.GetDimensions(), cam.Transform(Vec2I(HitBox.left , HitBox.top()))), Colors::Green, Effects::Copy{});

		pWeapon->Draw(gfx, cam);*/
	}

	void Player::UpdateMovement(World& world, const chili::Keyboard& kbd, float time)
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
			e2d::Vec2I new_pos = e2d::Vec2I(HitBox.left - e2d::Renderer::ScreenWidth / 2, HitBox.bottom - e2d::Renderer::ScreenHeight / 2);
			camera.SetPos(new_pos);
		}

	}



	void Player::GiveWeapon(std::unique_ptr<Weapon> wp)
	{
		pWeapon = std::move(wp);
	}

	void Player::UseWeapon(float time)
	{
		if (mouse.LeftIsPressed())
		{
			e2d::Vec2I mouse_pos = e2d::CoordinateShower::DetermineCoordinates(mouse, camera);
			pWeapon->Use(e2d::Vec2D(float(std::move(mouse_pos.x)), float(std::move(mouse_pos.y))));
		}
		pWeapon->Update(time);
	}

	const std::forward_list<e2d::Rect> Player::GetProj_list()
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
}