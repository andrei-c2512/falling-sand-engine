#pragma once
#include <limits>
#include <functional>
#include <list>
#include <algorithm>
#include <iterator>
#include "engine2d/pipeline/Camera.hpp"
#include "falling-sand/World.hpp"
#include "falling-sand/Weapon.hpp"

#define INFINITY 100000

/*
struct aNode {
	void Init(const Vec2I& pos0 , bool IsObs0 )
	{
		pos = pos0;
		IsObstacle = IsObs0;
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
public:
	// This algorthim uses REAL WORLD coords and not the falling sand matrix coords
	template<typename T>
	static std::vector<Vec2I> ApplyAlgorithm(Vec2_<T> start, Vec2_<T> end, World& world,
											Mouse& mouse , Graphics& gfx, const RectI& rect = Graphics::GetScreenRect())
	{
		std::vector<aNode> node_list(rect.width * rect.height);

		rect = rect / World::ElemSize;
		Vec2I offset = Vec2I(rect.left / World::ElemSize, rect.top / World::ElemSize);
		start /= World::ElemSize;
		end /= World::ElemSize;

		auto dim = world.GetSandboxDim();
		for (int y = rect.bottom; y < rect.top(); y += 1)
		{
			for (int x = rect.left; x < rect.right(); x += 1)
			{
				const int index = y * rect.width + x;
				auto MatrixIndex = world.GetElemIndScr(Vec2I(x, y));

				bool IsObs = (world.GetElem(MatrixIndex)->GetState() == State::Solid);

				node_list[index].Init(Vec2I(x, y), IsObs);


				//for (int y0 = -1; y0 <= 1; y0++)
				//{
				//	for (int x0 = -1; x0 <= 1; x0++)
				//	{
				//		if (x0 == 0 && y0 == 0)
				//		{
				//			continue;
				//		}
				//		bool Add = true;
				//		int deltaX = x0;
				//		int deltaY = y0 * dim.width;
				//
				//		if (x == rect.left && x0 == -1)
				//		{
				//			Add = false;
				//		}
				//		else if (x == rect.right() - 1 && x0 == 1)
				//		{
				//			Add = false;
				//		}
				//
				//		if (y == rect.top && y0 == -1)
				//		{
				//			Add = false;
				//		}
				//		else if (y == rect.bottom() - 1 && y0 == 1)
				//		{
				//			Add = false;
				//		}
				//
				//		int new_ind = index + deltaX + deltaY;
				//		if (Add)
				//		{
				//			assert(new_ind >= 0);
				//			auto* node = &node_list[new_ind];
				//			node_list[index].Neighbours.emplace_back(node);
				//		}
				//
				//	}
				//}
	
				if (x - 1 >= 0)
				{
					node_list[index].Neighbours.emplace_back(&node_list[index - 1]);
				}
				if (y - 1 >= 0)
				{
					node_list[index].Neighbours.emplace_back(&node_list[index - dim.width]);
				}
				if (y + 1 < rect.height )
				{
					node_list[index].Neighbours.emplace_back(&node_list[index + dim.width]);
				}
				if (x + 1 < rect.width)
				{
					node_list[index].Neighbours.emplace_back(&node_list[index + 1]);
				}

			}
		}
		
		auto mouse_pos = mouse.GetPos();
		mouse_pos /= World::ElemSize;

		int node_index = mouse_pos.y * rect.width + mouse_pos.x;

		for (auto& n : node_list[node_index].Neighbours)
		{
			gfx.DrawRect(RectI(World::ElemSize, World::ElemSize, Vec2I(n->pos * World::ElemSize)), Colors::Red);
		}

		std::function<float(aNode*, aNode*)> heuristic = [](aNode* n1, aNode* n2) {
			return n1->pos.GetLenght(n2->pos);
		};
		
		aNode* start_node = &node_list[((int(start.y) - offset.y) / World::ElemSize) * rect.width + (int(start.x) - offset.x)];
		aNode* end_node   = &node_list[((int(end.y) - offset.y) / World::ElemSize)   * rect.width + (int(end.x) - offset.x)];
		
		start_node->lDist = 0.0f;
		start_node->gDist = heuristic(start_node , end_node);
		
		std::vector<aNode*> test_list;
		
		test_list.push_back(start_node);
		aNode* CurrentNode = nullptr;
		
		while (test_list.empty() == false && end_node->parent == nullptr)
		{
			std::sort(std::begin(test_list) , std::end(test_list) ,[](const aNode* n1, const aNode* n2) {
				return n1->gDist < n2->gDist;
				});
		
			if (test_list.back()->Verrified == true)
			{
				test_list.pop_back();
			}
		
			if (test_list.empty())
			{
				break;
			}
		
			CurrentNode = test_list.front();
		
			CurrentNode->Verrified = true;
			for (aNode* neighbour : CurrentNode->Neighbours)
			{
				if (neighbour->Verrified == false)
				{
					auto Dist = CurrentNode->lDist + heuristic(CurrentNode, neighbour);
		
					if (std::find_if(std::begin(test_list), std::end(test_list), [=](aNode* node)
						{
							return node == neighbour;
						}) == std::end(test_list))
					{
						test_list.emplace_back(neighbour);
					}
		
					if (Dist < neighbour->lDist)
					{
						neighbour->lDist = Dist;
						neighbour->gDist = Dist + heuristic(CurrentNode, end_node);
						neighbour->parent = CurrentNode;
					}
				}
			}
 		
		}

		std::vector<Vec2I> pos_list; // point list


		while (end_node != nullptr)
		{
			pos_list.emplace_back(end_node->pos);
			end_node = end_node->parent;
		}
		
		std::reverse(std::begin(pos_list), std::end(pos_list));

		return pos_list;
	}
};
*/

namespace sand {
	class Entity {
	public:
		Entity() = default;
		Entity(e2d::Rect rect, e2d::Sprite sprite, float dmg, int health, float speed);
		Entity(const Entity& entity);
		virtual void Draw(e2d::Renderer& gfx, const e2d::Camera& ct);
		void SetPos(e2d::Vec2I& pos);
		void Move(World& world, float time);
		void MoveX(World& world, float time);
		void CheckProtrusionY(World& world, float y);
		void MoveY(World& world, float time);

		void MoveUp(World& world, float time);
		void MoveDown(World& world, float time);
		void MoveRight(World& world, float time);
		void MoveLeft(World& world, float time);
		void Kill(World& world);
		virtual bool CheckCollision(Entity& entity); // virtual because here you decide if the children classes
		// will have only body collision , or only missile collisions etc

		bool CheckBodyCollision(Entity& entity);  // checks if only the bodies of the entities collided
		bool CheckWeaponCollision(Entity& entity); // checks if the weapon collided with the entity passed
		bool CheckFullCollision(Entity& entity);  // checks if the body collided with the passed entity body and its weapon
		void DamageEntity(int damage);
	public:
		//getters
		e2d::Rect GetRect() const;
		bool IsDead() const { return Dead; };
	protected:
		static constexpr float Gravity = 10.0f;
		std::unique_ptr<Weapon> pWeapon = nullptr;
		e2d::Rect HitBox;
		e2d::Sprite sprite;

		e2d::Vec2D vel = { 0.0f , 0.0f };
		float Damage = 0;
		float Speed = 0;

		bool Dead = false;

		struct HpBar {
			HpBar() = default;
			int HpBarWidth() const
			{
				return int(float(dim.width * (float(Health) / float(MaxHealth))));
			};
			static constexpr int offset = 5; //this is for elongating the bar

			e2d::Timer show_timer = 4.0f;
			e2d::Size<short> dim = { 20 , 5 };
			int MaxHealth = 100;
			int Health = MaxHealth;
		};

		HpBar hp_bar;
	};
}