#pragma once
#include "Vec2D.h"

class Block {
public:
	Block() = default;
	Block(Vec2D& pos0, int width0, int height0);
	void SpawnBlock(Vec2D pos0);
	void Collision(Vec2D pos0, Vec2D vel0, float speed);
private:
	Vec2D pos;
	int width;
	int height;
};