#pragma once
#include "engine2d/pipeline/CoordinateTransformer.hpp"
#include "engine2d/core/Graphics.hpp"
#include "chili/Mouse.hpp"

namespace e2d {
	class Camera {
	public:
		Camera(CoordinateTransformer& ct0)
			:ct(ct0)
		{
			//Vec2I camera_pos = Vec2I(Graphics::ScreenWidth / 2 , Graphics::ScreenHeight / 2 );
			WorldPos = { -800 , -600 };
			ZonePos = Vec2D(0.0f, 0.0f);
			OffLimitsZone = RectI(Renderer::ScreenWidth + 2 * OffLimitDist, Renderer::ScreenHeight + 2 * OffLimitDist,
				Vec2I(-OffLimitDist, -OffLimitDist));

			TargetPos = { 0 , 0 };
			vel = { 0.0f , 0.0f };
		}
		int TransformX(int x) const
		{
			return ct.TransformX(x - WorldPos.x);
		}
		int TransformY(int y) const
		{
			return ct.TransformY(y - WorldPos.y);
		}
		template <typename T>
		Vec2I Transform(Vec2_<T> pos0) const
		{
			Vec2I cPos = Vec2I(pos0.x, pos0.y);
			return ct.Transform(Vec2I(cPos - GetPos()));
		}
		//transforms only to the camera
		template <typename T>
		Vec2I CameraTransformation(Vec2_<T> pos0)
		{
			Vec2I cPos = Vec2I(pos0.x, pos0.y);
			return Vec2I(cPos - GetPos());
		}
		template <typename T>
		void MoveBy(Vec2_<T> pos0)
		{
			Vec2I cPos = Vec2I(pos0.x, pos0.y);
			WorldPos = WorldPos + cPos;
		}
		void MoveLeft(int x)
		{
			assert(x < 0);
			if (WorldPos.x + x >= Renderer::WorldArea.left)
			{
				WorldPos.x += x;
			}
		}
		void MoveRight(int x)
		{
			assert(x > 0);
			if (WorldPos.x + Renderer::ScreenWidth + x < Renderer::WorldArea.right())
			{
				WorldPos.x += x;
			}
		}
		void MoveUp(int y)
		{
			assert(y > 0);
			if (WorldPos.y + Renderer::ScreenHeight + y < Renderer::WorldArea.top())
			{
				WorldPos.y += y;
			}
		}
		void MoveDown(int y)
		{
			assert(y < 0);
			if (WorldPos.y + y >= Renderer::WorldArea.bottom)
			{
				WorldPos.y += y;
			}
		}
		template<typename T>
		bool WithinWorldX(T x) const {
			return int(x) >= Renderer::WorldArea.left && int(x) + Renderer::ScreenWidth < Renderer::WorldArea.right();
		}
		template<typename T>
		bool WithinWorldY(T y) const {
			return int(y) >= Renderer::WorldArea.bottom && int(y) + Renderer::ScreenHeight < Renderer::WorldArea.top();
		}
		template <typename T>
		void SetPos(Vec2_<T> pos0)
		{
			//checking if in bounds on x axis
			if (WithinWorldX(pos0.x))
			{
				WorldPos.x = std::move(pos0.x);
			}
			else
			{
				int dist_left = std::abs(Renderer::WorldArea.left - int(pos0.x));
				int dist_right = std::abs(Renderer::WorldArea.right() - int(pos0.x));

				if (dist_left < dist_right)
				{
					WorldPos.x = Renderer::WorldArea.left;
				}
				else
				{
					WorldPos.x = Renderer::WorldArea.right() - Renderer::ScreenWidth;
				}
			}

			//checking if in bounds on y axis
			if (WithinWorldY(pos0.y))
			{
				WorldPos.y = std::move(pos0.y);
			}
			else
			{
				int dist_top = std::abs(Renderer::WorldArea.bottom - int(pos0.y));
				int dist_bottom = std::abs(Renderer::WorldArea.top() - int(pos0.y));

				if (dist_top < dist_bottom)
				{
					WorldPos.y = Renderer::WorldArea.bottom;
				}
				else
				{
					WorldPos.y = Renderer::WorldArea.top() - Renderer::ScreenHeight;
				}
			}
			CheckIfOffBounds();
		}
		void MouseMovement(const chili::Mouse& mouse)
		{
			//converting mouse pos to world pos
			Vec2I mouse_pos = mouse.GetPos();
			mouse_pos.y = Renderer::ScreenHeight - mouse_pos.y;
			Vec2I world_pos = mouse_pos + WorldPos + Vec2I(int(ZonePos.x), int(ZonePos.y));

			int distX = (WorldPos.x - OffLimitDist) + OffLimitsZone.width / 2 - world_pos.x;

			//idk why i put minus before dist , when I did the camera system when looking left it shifted to the right and vice versa
			//it just works
			if (world_pos.x > (WorldPos.x - OffLimitDist) + OffLimitsZone.width / 2)
			{
				TargetPos.x = -distX / MoveRatioX;

				vel.x = (TargetPos.x - ZonePos.x) / MoveFactor;
			}
			else
			{
				TargetPos.x = -distX / MoveRatioX;

				vel.x = (TargetPos.x - ZonePos.x) / MoveFactor;
			}


			int distY = (WorldPos.y - OffLimitDist) + OffLimitsZone.height / 2 - world_pos.y;

			if (world_pos.y > (WorldPos.y - OffLimitDist) + OffLimitsZone.height / 2)
			{
				TargetPos.y = -distY / MoveRatioY;

				vel.y = (TargetPos.y - ZonePos.y) / MoveFactor;
			}
			else
			{
				TargetPos.y = -distY / MoveRatioY;

				vel.y = (TargetPos.y - ZonePos.y) / MoveFactor;
			}
		}
		void UpdateMovement(float time)
		{
			float AddX = time * vel.x * 60.0f;
			if (WithinWorldX(GetX() + AddX) && std::abs(ZonePos.x + AddX) < OffLimitDist)
			{
				ZonePos.x += AddX;
			}
			vel.x *= DecreaseFactor * (1.0f - time);

			float AddY = time * vel.y * 60.0f;
			if (WithinWorldY(GetY() + AddY) && std::abs(ZonePos.y + AddY) < OffLimitDist)
			{
				ZonePos.y += AddY;
			}
			vel.y *= DecreaseFactor * (1.0f - time);
		}
		//this checks if the camera is outside the camera boundaries because of its position in the zone
		void CheckIfOffBounds()
		{
			if (GetX() + Renderer::ScreenWidth >= Renderer::WorldArea.right())
			{
				ZonePos.x -= GetX() + Renderer::ScreenWidth - Renderer::WorldArea.right();
			}

			if (GetY() + Renderer::ScreenHeight >= Renderer::WorldArea.top())
			{
				ZonePos.y -= GetY() + Renderer::ScreenHeight - Renderer::WorldArea.top();
			}

			if (GetX() < Renderer::WorldArea.left)
			{
				ZonePos.x += Renderer::WorldArea.left - GetX() - 1;
			}

			if (GetY() < Renderer::WorldArea.bottom)
			{
				ZonePos.y += Renderer::WorldArea.bottom - GetY() - 1;
			}
		}
		template <typename T>
		Vec2I ToCamera(Vec2_<T> pos0) const
		{
			return ct.Transform(pos0);
		}
		Vec2I GetPos()const {
			return Vec2I(WorldPos.x + ZonePos.x, WorldPos.y + ZonePos.y);
		}
		int GetX() const {
			return WorldPos.x + ZonePos.x;
		}
		int GetY() const {
			return WorldPos.y + ZonePos.y;
		}
		void UpdateZone()
		{
			OffLimitsZone.SetPos(Vec2I(WorldPos.x - OffLimitDist, WorldPos.y - OffLimitDist));
		}
		static constexpr  int OffLimitDist = 100;
		static constexpr  float MoveFactor = 100.0f;
		static constexpr  float DecreaseFactor = 0.6f;
		static constexpr float MoveRatioX = (Renderer::ScreenWidth / 2) / OffLimitDist;
		static constexpr float MoveRatioY = (Renderer::ScreenHeight / 2) / OffLimitDist;
	private:
		Vec2I WorldPos;
		Vec2D ZonePos;

		//taget pos in the offlimits zone
		Vec2I TargetPos;
		RectI OffLimitsZone;

		Vec2D vel;

		CoordinateTransformer& ct;
	};
}