#pragma once
#include "Entity.h"

class NPC : public Entity {
public:
	NPC() = default;
	NPC(const Entity& entity);
	bool CheckCollision(Entity& entity) override;
	void DetermineMovement();
	void UpdateWeapon(float time);
	void ChangeTarget(Entity* newtarget);
protected:
	Entity* pTarget = nullptr;
};
