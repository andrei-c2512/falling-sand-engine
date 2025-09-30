#include "falling-sand/NPC.hpp"

namespace sand {
	NPC::NPC(const Entity& entity)
		:Entity(entity)
	{
	}
	void NPC::DetermineMovement()
	{
		if (pTarget != nullptr)
		{
			e2d::Rect pRect = pTarget->GetRect();

			e2d::Vec2D pCenter = pRect.GetCenter(); // player center
			e2d::Vec2D eCenter = HitBox.GetCenter(); // enemy center

			float hypothenuse = pCenter.GetLenght(eCenter);
			float sin = eCenter.GetSin(pCenter, hypothenuse);
			float cos = eCenter.GetCos(pCenter, hypothenuse);

			vel = { cos * Speed , sin * Speed };
			//assert(!(vel.x == 0.0f && vel.y == 0.0f));
		}
	}

	bool NPC::CheckCollision(Entity& entity)
	{
		if (Entity::CheckCollision(entity) == true)
		{
			pTarget = &entity;
			return true;
		}
		return false;
	}

	void NPC::UpdateWeapon(float time)
	{
		if (pTarget != nullptr)
		{
			e2d::Vec2D target_pos = pTarget->GetRect().GetCenter();
			pWeapon->Update(time);
			pWeapon->Use(target_pos);
		}
	}

	void NPC::ChangeTarget(Entity* newtarget)
	{
		pTarget = newtarget;
	}
}