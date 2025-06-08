#include "Ball.h"

Ball::Ball(Vec2D pos0, int radius)
	:pos(std::move(pos0)) , Radius(radius)
{
	velocity = { 0.0f , 0.0f };
	Closest_segment = { Vec2I(0 , 0) , Vec2I(0,0) };
	ClosestDist = std::numeric_limits<float>::infinity();
}
void Ball::ShowClosestDistance(Graphics& gfx, Camera& camera)
{
	if (Closest_segment.first != Closest_segment.second)
	{
		Vec2D projection = GetProjectionOnLine(Closest_segment.first, Closest_segment.second, pos);
		if (PointOnSegment(Closest_segment.first, Closest_segment.second, projection))
		{
			gfx.DrawLine(camera.Transform(pos), camera.Transform(projection), Colors::Green);
		}
		else
		{
			Vec2D point = ClosestPoint(Closest_segment.first, Closest_segment.second, pos).first;
			gfx.DrawLine(camera.Transform(pos), camera.Transform(point), Colors::Green);
		}

		std::string text = "Distance to closest segment: " + std::to_string(ClosestDist);

		int textX = -Graphics::ScreenWidth / 2;
		int textY = Graphics::ScreenHeight / 2 - font.GetLetterDim().height;

		font.DrawWords(text, gfx, camera, Vec2I(std::move(textX), std::move(textY)));
	}
}
void Ball::LaunchBall(Camera& camera , Mouse& mouse)
{
	//assert(Launched == false);

	//if (Launched == false)
	{
		if (mouse.LeftIsPressed())
		{
			Launched = true;

			Vec2I MousePos = mouse.GetPos();
			MousePos = CoordinateShower::DetermineCoordinates(mouse, camera);

			Vec2I BallPos = Vec2I(int(pos.x), int(pos.y));

			float hyphotenuse = BallPos.GetLenght(MousePos);

			float sin = BallPos.GetSin(MousePos, hyphotenuse);
			float cos = BallPos.GetCos(MousePos, hyphotenuse);

			velocity = Vec2D(cos * BaseSpeed, sin * BaseSpeed);
		}
	}
	//else
	{
		if (mouse.RightIsPressed())
		{
			velocity = { 0.0f , 0.0f };
			pos = { 0.0f , 0.0f };
			Launched = false;
		}
	}
}


void Ball::DrawBall(Graphics& gfx, Camera& camera)
{
	RectI Area = RectI(Radius * 2, Radius * 2, Vec2I(pos.x - Radius, pos.y - Radius));

	Vec2I Center(int(pos.x) , int(pos.y));
	for (int y = Area.bottom ; y < Area.top(); y++)
	{
		for (int x = Area.left; x < Area.right(); x++)
		{
			Vec2I point(x, y);

			int lenght = int(point.GetLenght(Center));

			if (lenght <= Radius && lenght >= Radius - 4)
			{
				gfx.PutPixel(camera.Transform(std::move(point)), Colors::Cyan);
			}
		}
	}

}

bool Ball::BorderCollision(float time)
{
	bool Collision = false;
	if (pos.x - Radius + velocity.x * time < Graphics::WorldArea.left)
	{
		velocity.x *= -1.0f;
		pos.x = Graphics::WorldArea.left + Radius;
		Collision = true;
	}

	if (pos.x + Radius + velocity.x * time >= Graphics::WorldArea.right())
	{
		velocity.x *= -1.0f;
		pos.x = Graphics::WorldArea.right() - Radius - 1;
		Collision = true;
	}


	if (pos.y - Radius + velocity.y * time < Graphics::WorldArea.bottom)
	{
		velocity.y *= -1.0f;
		pos.y = Graphics::WorldArea.bottom + Radius;
		Collision = true;
	}

	if (pos.y + Radius + velocity.y * time >= Graphics::WorldArea.top())
	{
		velocity.y *= -1.0f;
		pos.y = Graphics::WorldArea.top() - Radius - 1;
		Collision = true;
	}

	return Collision;
}


void Ball::UpdateMovement(FigureField& field , Camera& cam ,float time)
{
	if (BorderCollision(time) == false && CheckCollision(field, time) == false)
	{ 
		pos.y += velocity.y * time * 60.0f;
		pos.x += velocity.x * time * 60.0f;
		UpdateDist();
	}
}

bool Ball::CheckCollision(FigureField& field ,float time)
{
	short chunk_width = field.GetChunkWidth();
	short chunk_height = field.GetChunkHeight();

	Vec2I ChunkPos = Vec2I(pos.x / chunk_width + field.GetColumns() / 2, pos.y / chunk_height + field.GetRows() / 2);

	Vec2I PositionWithinChunk = Vec2I(int(pos.x) % chunk_width, int(pos.y) % chunk_height);

	Vec2D add = { velocity.x * time * 60.0f, velocity.y * time * 60.0f};
	Vec2I projected_pos = Vec2I( pos.x + add.x, pos.y + add.y );
	std::vector<Vec2I> ChunkPos_list = { ChunkPos };

	int x = ChunkPos.x;
	char deltaX = 0;
	if (PositionWithinChunk.x <= chunk_width / 2)
	{
		if(x - 1 >= 0)
			deltaX = -1;
	}
	else
	{
		if (x + 1 < field.GetColumns())
			deltaX = 1;
	}

	int y = ChunkPos.y;
	char deltaY = 0;
	if (PositionWithinChunk.y <= chunk_height / 2)
	{
		if (y - 1 >= 0)
			deltaY = -1;
	}
	else
	{
		if (y + 1 < field.GetRows())
			deltaY = 1;
	}

	ChunkPos_list.emplace_back(Vec2I(x + int(deltaX), y));
	ChunkPos_list.emplace_back(Vec2I(x              , y + int(deltaY)));
	ChunkPos_list.emplace_back(Vec2I(x + int(deltaX), y + int(deltaY)));

	SpaceChunk* list = field.ChunkListData();
	RectI HitBox = RectI(Radius * 2, Radius * 2, Vec2I(projected_pos.x - Radius, projected_pos.y - Radius));
	const Figure* figure = nullptr;

	for (const Vec2I& position : ChunkPos_list)
	{
		unsigned int index = (position.y ) * field.GetColumns() + (position.x );
		
		assert(index >= 0 && index < unsigned(field.GetColumns() * field.GetRows()));
		figure = list[index].CheckBoxCollision(HitBox);

		if (figure != nullptr)
		{
			break;
		}
	}


	if (figure != nullptr)
	{
 		std::vector<Vec2I> current_model = figure->GetCurrentModelSize();
		Vec2I fig_pos = figure->GetPos();

		for (unsigned int ind = 1; ind < current_model.size() - 1; ind++)
		{
  			Vec2I p1 = current_model[ind - 1] + fig_pos;
			Vec2I p2 = current_model[ind    ] + fig_pos;
			Vec2I p3 = current_model[ind + 1] + fig_pos;

			bool rebound = ChooseLineForRebound(p1, p2, p3, projected_pos, time);

			if (rebound)
				return true;
		}

		Vec2I p1 = current_model[current_model.size() - 2] + fig_pos;
		Vec2I p2 = current_model[current_model.size() - 1] + fig_pos;
		Vec2I p3 = current_model[0] + fig_pos;
		bool rebound = ChooseLineForRebound(p1, p2, p3, projected_pos, time);

		if (rebound)
			return true;


		p1 = current_model[current_model.size() - 1] + fig_pos;
		p2 = current_model[0] + fig_pos;
		p3 = current_model[1] + fig_pos;
		rebound = ChooseLineForRebound(p1, p2, p3, projected_pos, time);

		if (rebound)
			return true;
	}
	return false;
}

bool Ball::CheckForRebound(Vec2I p1, Vec2I p2 , float time)
{
	//if (IsHeadingTowardsLine(p1 , p2 , pos , velocity) )
	{
		Vec2D P1(p1.x, p1.y);
		Vec2D P2(p2.x, p2.y);

		Vec2D projected_pos = pos + velocity * time * 60.0f;
		float dist = DistanceToSegment(P1, P2, projected_pos);

		if (dist < ClosestDist)
		{
			Closest_segment = std::pair<Vec2I, Vec2I>(p1, p2);
			ClosestDist = dist;
		}

		if (dist <= Radius)
		{
			Vec2I line_vector = p2 - p1;
			Vec2D w(line_vector.GetNormalizedVector());

			{
				float dot_product = w * velocity;
				Vec2D k = w * dot_product;
				Vec2D j = velocity - k;
				j.x *= 2.0f;
				j.y *= 2.0f;
				velocity = velocity - j;
			}
			return true;
		}
	}
	return false;
}

bool Ball::ChooseLineForRebound(Vec2I p1, Vec2I p2, Vec2I p3, Vec2I obj , float time)
{
	//float previous_dist = DistanceToSegment(p1, p2, obj);
	//float current_dist =  DistanceToSegment(p2, p3, obj);
	//
	//bool rebound = false;
	//
	////rebound to the closest distance or if the distance between the P1P2 segment (previous_dist)
	////is equal to the distance to one of the edges , then give priority to the other distance
	//if ( previous_dist == (ClosestPoint(p1 , p2 , obj).second))
	//{
	//	rebound = CheckForRebound(p2, p3, time);
	//}
	//else if(current_dist == (ClosestPoint(p2, p3, obj).second))
	//{
	//	rebound = CheckForRebound(p1 , p2, time);
	//}
	//else
	//{
	//	if (current_dist < previous_dist)
	//	{
	//		rebound = CheckForRebound(p2, p3, time);
	//	}
	//	else
	//	{
	//		rebound = CheckForRebound(p1, p2, time);
	//	}
	//}
	//
	//
	//return rebound;

	Vec2I vec1 = p2 - p1;
	Vec2I vec2 = p2 - p3;

	Vec2I combined_vector = vec1 + vec2;

	if (combined_vector.x == 0)
	{
		combined_vector += p2;

		float limit_x = combined_vector.x;
		Vec2I rhs = p1;
		Vec2I lhs = p3;

		if (p1.x < p3.x)
		{
			std::swap(rhs, lhs);
		}
		if (obj.x < limit_x)
		{
			if (CheckForRebound(p2, lhs, time) == false)
			{
				return CheckForRebound(p2, rhs, time);
			}
			else
			{
				return true;
			}
		}
		else if(obj.x > limit_x)
		{
			if (CheckForRebound(p2, rhs, time) == false)
			{
				return CheckForRebound(p2, lhs, time);
			}
			else
			{
				return true;
			}
		}
		else
		{

			float dist = obj.GetLenght(p2);

			if (dist <= Radius)
			{
				velocity *= -1.0f;
				return true;
			}
			return false;
		}
	}
	else if (combined_vector.y == 0)
	{
		combined_vector += p2;

		float limit_y = combined_vector.y;
		Vec2I up = p1;
		Vec2I down = p3;

		if (p3.y > p1.y)
		{
			std::swap(p1, p3);

		}
		if (obj.y < limit_y)
		{
			if (CheckForRebound(p2, down, time) == false)
			{
				return CheckForRebound(p2, up, time);
			}
			else
			{
				return true;
			}
		}
		else if (obj.y > limit_y)
		{
			if (CheckForRebound(p2, up, time) == false)
			{
				return CheckForRebound(p2, down, time);
			}
			else
			{
				return true;
			}
		}
		else
		{

			float dist = obj.GetLenght(p2);

			if (dist <= Radius)
			{
				velocity *= -1.0f;
				return true;
			}
			return false;
		}

	}
	else
	{
		float slope = float(combined_vector.y) / float(combined_vector.x);
		float offset = p2.y - p2.x * slope;

		float limit_x = (obj.y - offset) / slope;


		Vec2I rhs = p1;
		Vec2I lhs = p3;

		if (p1.x < p3.x)
		{
			std::swap(rhs, lhs);
		}
		if (obj.x < limit_x)
		{
			if (CheckForRebound(p2, lhs, time) == false)
			{
				return CheckForRebound(p2, rhs, time);
			}
			else
			{
				return true;
			}
		}
		else if(obj.x > limit_x)
		{
			if (CheckForRebound(p2, rhs, time) == false)
			{
				return CheckForRebound(p2, lhs, time);
			}
			else
			{
				return true;
			}
		}
		else
		{

			float dist = obj.GetLenght(p2);

			if (dist <= Radius)
			{
				velocity *= -1.0f;
				return true;
			}
			return false;
		}
	}
	return false;
}

void Ball::UpdateDist()
{
	if(Closest_segment.first != Closest_segment.second)
	ClosestDist = DistanceToSegment(Closest_segment.first, Closest_segment.second, pos);
}