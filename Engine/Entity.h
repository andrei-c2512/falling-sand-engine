#pragma once
#include "World.h"
#include "Player.h"
#include <limits>
#include <functional>
#include <list>
#define INFINITY (unsigned)!((int)0)

struct aNode {
	void Init(const Vec2I& pos0 , bool IsObs )
	{
		pos = pos0;
		Verrified = false;
		parent = nullptr;
		gDist = INFINITY;
		lDist = INFINITY;
	}
	bool IsObstacle; // is it an obstacle and can we not pass through it
	bool Verrified = false;
	int gDist; // distance that has been passed + remaining distance AKA global distance
	int lDist; // distance that has been passed so far AKA local distance;

	Vec2I pos;
	std::vector<aNode*> Neighbours;
	aNode* parent = nullptr; // a node will have a parent meaning THIS point will be the next destination ,
	// after the parent node
};
class AStarAlgorithm {

	// This algorthim uses REAL WORLD coords and not the falling sand matrix coords
	static std::vector<Vec2I> ApplyAlgorithm(Vec2I start, Vec2I end, World& world, RectI& rect = Graphics::GetScreenRect())
	{
		std::vector<aNode> node_list(rect.width * rect.height);

		rect = rect / World::ElemSize;
		Vec2I offset = Vec2I(rect.left / World::ElemSize, rect.top / World::ElemSize);
		start /= World::ElemSize;
		end /= World::ElemSize;

		auto dim = world.GetSandboxDim();
		for (int y = rect.top; y < rect.bottom(); y += 1)
		{
			for (int x = rect.left; x < rect.right(); x += 1)
			{
				const unsigned int index = y * rect.width + x;
				auto MatrixIndex = world.GetElemIndScr(Vec2I(x, y));

				bool IsObs = (world.GetElem(MatrixIndex)->GetState() == State::Solid);

				node_list[index].Init(Vec2I(x, y), IsObs);


				for (int y0 = -1; y0 <= 1; y0++)
				{
					for (int x0 = -1; x0 <= 1; x0++)
					{
						int deltaX = x0;
						int deltaY = y0 * dim.width;

						if (x == rect.left && x0 == -1)
						{
							deltaX = 0;
						}
						else if (x == rect.right() - 1 && x0 == 1)
						{
							deltaX = 0;
						}

						if (y == rect.top && y0 == -1)
						{
							deltaY = 0;
						}
						else if (y == rect.bottom() - 1 && y0 == 1)
						{
							deltaY = 0;
						}

						node_list[index].Neighbours.emplace_back(&node_list[index + deltaX + deltaY * rect.width]);

					}
				}
			}
		}

		std::function<float(aNode*, aNode*)> heuristic = [](aNode* n1, aNode* n2) {
			return n1->pos.GetLenght(n2->pos);
		};

		
		aNode* start_node = &node_list[((start.y - offset.y) / World::ElemSize) * rect.width + (start.x - offset.x)];
		aNode* end_node   = &node_list[((end.y - offset.y) / World::ElemSize) * rect.width + (end.x - offset.x)];

		start_node->lDist = 0.0f;
		start_node->gDist = heuristic(start_node , end_node);

		std::list<aNode*> test_list;
		
		test_list.push_back(start_node);
		aNode* CurrentNode = nullptr;

		while (test_list.empty() == false)
		{
			test_list.sort([](const aNode* n1 , const aNode* n2) {
				return n1->gDist > n2->gDist;
				});

			if (test_list.front()->Verrified == true)
			{
				test_list.pop_front();
			}

			if (test_list.empty())
			{
				break;
			}

			CurrentNode = test_list.front();

			CurrentNode->Verrified = true;

			for (auto neighbour : CurrentNode->Neighbours)
			{
				if (neighbour->Verrified == false)
				{
					auto Dist = CurrentNode->lDist + heuristic(CurrentNode, neighbour);
					if (Dist < neighbour->lDist)
					{
						if (neighbour->lDist == INFINITY)
						{
							test_list.emplace_back(neighbour);
						}

						neighbour->lDist = Dist;
						neighbour->gDist = Dist + heuristic(CurrentNode, end_node);
						neighbour->parent = CurrentNode;
					}
				}
			}
		}
	}
};

class Entity {
public:
	Entity(Rect rect, Sprite sprite, float dmg, int health, float speed);
	void Draw(Graphics& gfx);
	void DetermineMovement(Player& player);
	void Move(World& world ,float time);
	void MoveX(World& world , float time);
	void MoveY(World& world , float time);
	void CheckCollisions(Player& player);
private:
	static constexpr float Gravity = 10.0f;
	Rect HitBox;
	Sprite sprite;

	Vec2D vel;
	float Damage;
	int   Health;
	float Speed;
};