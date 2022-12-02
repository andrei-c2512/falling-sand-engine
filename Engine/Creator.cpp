#include "Creator.h"
#include "assert.h"
Creator::Creator(RectI& ButtonSize, int Radius , World& world , Weather& weather0)
	:world(world), Chance(1 , 100),weather(weather0),explosion(Explosion(world, 30))
{
	SpawnRadius = 15;
	ToBeSpawned = Type::Steam;

	int space = 10;
	for (size_t i = 0; i < (int)Type::Count ; i++)
	{
		int x = ElemButtonPos.x + i * (ElementButton::dim + space);
		EButtons.emplace_back(Vec2I{ x , ElemButtonPos.y },Type(i));
	}

	for (size_t i = 0; i < (int)WeatherType::Count; i++)
	{
		int x = WeatherButtonPos.x + i * (ElementButton::dim + space);
		WButtons.emplace_back(Vec2I{ x , WeatherButtonPos.y }, WeatherType(i));
	}
}


void Creator::Spawn(Mouse& mouse ,MouseLastFrameStats& previous_stats, Sandbox& sandbox ,ParticleEffect& list)
{
	if (!IsHoveringAButton(mouse))
	{
		if (ToBeSpawned != Type::Explosion)
		{
			if (mouse.LeftIsPressed())
			{
				auto elem_list = GetSpawnableElements(mouse);
				for (auto& n : elem_list)
				{
					if (ToBeSpawned == Type::Water)
					{
						if (Chance.GetVal() > 50)
						{
							if (world.CreateElem(n, ToBeSpawned) == true)
								sandbox.ActivateChunk(n);
						}
					}
					else
					{
						if (world.CreateElem(n, ToBeSpawned) == true)
							sandbox.ActivateChunk(n);
					}
				}
			}
			else if (mouse.RightIsPressed())
			{
				if (previous_stats.RightIsPressed())
				{

				}
				else
				{
					SpawnLoc = mouse.GetPos();
				}
			}
			else if (!mouse.RightIsPressed() && previous_stats.RightIsPressed())
			{
				Vec2I last_pos = previous_stats.GetPos();
				RectI Zone = RectI(world.ScreenToMatrixPos(SpawnLoc),
					world.ScreenToMatrixPos(last_pos));

				auto dim = world.GetSandboxDim();
				for (int y = Zone.top; y != Zone.bottom(); y += 1)
				{
					for (int x = Zone.left; x != Zone.right(); x += 1)
					{
						size_t index = y * dim.width + x;
						world.CreateElem(index, ToBeSpawned);
						sandbox.ActivateChunk(index);
					}
				}

			}
		}
		else
		{
			if (mouse.LeftIsPressed())
			{
				auto MousePos = mouse.GetPos();
				explosion.ExplodeZone(MousePos , list);
			}

		}
		
	}
}

void Creator::DrawButtons(Graphics& gfx)
{
	for (size_t i = 0; i < (int)Type::Count; i++)
	{
		EButtons[i].Draw(gfx);
	}
	for (size_t i = 0; i < (int)WeatherType::Count; i++)
	{
		WButtons[i].Draw(gfx);
	}
}


bool Creator::CheckButtons(Mouse& mouse)
{
	if (mouse.LeftIsPressed())
	{
		for (size_t i = 0; i < (int)Type::Count; i++)
		{
			if (EButtons[i].IsHovered(mouse))
			{
				ToBeSpawned = EButtons[i].GetType();
				return true;
			}
		}
		for (size_t i = 0; i < (int)WeatherType::Count; i++)
		{
			if (WButtons[i].IsHovered(mouse))
			{
				weather.SetWeather(WButtons[i].GetType());
				return true;
			}
		}
	}
	return false;
}


void Creator::ShowHoveredElement(Mouse& mouse, Graphics& gfx)
{
	Vec2_<size_t> MousePos = Vec2_<size_t>( mouse.GetPosX() , mouse.GetPosY() );
	bool WithinScreen = (MousePos.x >= 0 && MousePos.x < Graphics::ScreenWidth) &&
		(MousePos.y >= 0 && MousePos.y < Graphics::ScreenHeight);

	if(WithinScreen)
	{
		Vec2_<size_t> WorldPos = Vec2_<size_t>( MousePos.x / World::ElemSize , MousePos.y / World::ElemSize );

		auto dim0 = world.GetSandboxDim();

		const int index = WorldPos.y * dim0.width + WorldPos.x;
		Type type = world.GetElem(index)->GetType();

		int Space = 10;

		auto dim1 = font.GetLetterDim();

		Vec2I InfoPos = Vec2I( 0  , Graphics::ScreenHeight - dim1.height - Space );

		auto& matrix = world.GetMatrix();
		std::vector<int> ElemCounter(int(Type::Count), 0);

		for (const auto& n : matrix)
		{
			int index = int(n.GetType());
			ElemCounter[index]++;
		}

		std::string ElemName = " ";
		switch (type)
		{
		case Type::Empty:
			ElemName = "Empty";
			break;
		case Type::Wood:
			ElemName = "Wood";
			break;
		case Type::Fire:
			ElemName = "Fire";
			break;
		case Type::Stone:
			ElemName = "Stone";
			break;
		case Type::Smoke:
			ElemName = "Smoke";
			break;
		case Type::Sand:
			ElemName = "Sand";
			break;
		case Type::Water:
			ElemName = "Water";
			break;
		case Type::Snow:
			ElemName = "Snow";
			break;
		case Type::Steam:
			ElemName = "Steam";
			break;
		case Type::ToxicGas:
			ElemName = "ToxicGas";
			break;
		case Type::Acid:
			ElemName = "Acid";
			break;
		}
		const std::string count = std::to_string(ElemCounter[int(type)]);

		InfoPos.x = Graphics::ScreenWidth - ElemName.size() * dim1.width - Space - (count.length() + 2) * dim1.width;
		const std::string result = ElemName + "(" + count + ")";
		font.DrawWords(result, gfx, InfoPos);
	}
}

void Creator::DrawSpawnSurface(Graphics& gfx , Mouse& mouse)
{

	if (!mouse.RightIsPressed())
	{
		int Radius = 0;
		Color circle_color;
		if (ToBeSpawned != Type::Explosion)
		{
			Radius = SpawnRadius;
			circle_color = Colors::Red;
		}
		else
		{
			Radius = ExplosionRadius;
			circle_color = Colors::Yellow;
		}

		auto WorldPos = world.ScreenToMatrixPos(mouse.GetPos());
		int dim = Radius * 2;
		RectI surf = RectI(dim, dim, Vec2I(WorldPos.x - Radius, WorldPos.y - Radius));

		std::vector<size_t> Elements;

		auto SandboxDim = world.GetSandboxDim();
		for (int y = surf.top; y < surf.bottom(); y++)
		{
			for (int x = surf.left; x < surf.right(); x++)
			{
				size_t ind = y * SandboxDim.width + x;
				if (ind < SandboxDim.GetArea())
				{
					Vec2I ElemLoc = world.IndexToPos(ind);

					float dist = ElemLoc.GetLenght(WorldPos);

					if (dist <= Radius && dist >= Radius - 1)
					{
						Elements.emplace_back(ind);
					}
				}
			}
		}

		for (auto& n : Elements)
		{
			RectI rect = world.GetElem(n)->GetRect();

			gfx.DrawRect(rect, circle_color);
		}
	}
	else
	{
		RectI Zone = RectI(mouse.GetPos(), SpawnLoc);
		gfx.DrawRect_Border(Zone, Colors::Green);
	}

}
bool Creator::IsHoveringAButton(Mouse& mouse) const
{
	for (size_t i = 0; i < (int)Type::Count; i++)
	{
		if (EButtons[i].IsHovered(mouse))
		{
			return true;
		}
	}
	for (size_t i = 0; i < (int)WeatherType::Count; i++)
	{
		if (WButtons[i].IsHovered(mouse))
		{
			return true;
		}
	}
	return false;
}
std::vector<size_t> Creator::GetSpawnableElements(Mouse& mouse)
{
	auto WorldPos = world.ScreenToMatrixPos(mouse.GetPos());

	int dim = SpawnRadius * 2;
	RectI surf = RectI(dim, dim, Vec2I(WorldPos.x - SpawnRadius, WorldPos.y - SpawnRadius));

	std::vector<size_t> Elements;

	auto SandboxDim = world.GetSandboxDim();
	for (int y = surf.top; y < surf.bottom(); y++)
	{
		for (int x = surf.left; x < surf.right(); x++)
		{
			size_t ind = y * SandboxDim.width + x;
			if (ind < SandboxDim.GetArea())
			{
				Vec2I ElemLoc = world.IndexToPos(ind);

				float dist = ElemLoc.GetLenght(WorldPos);

				if (dist <= SpawnRadius)
				{
					Elements.emplace_back(ind);
				}
			}
		}
	}

	return Elements;
}
