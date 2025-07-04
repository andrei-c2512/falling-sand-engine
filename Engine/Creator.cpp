#include "Creator.h"
#include "assert.h"
Creator::Creator(RectI& ButtonSize, int Radius , Simulation& world , Weather& weather0)
	:world(world), Chance(1 , 100),weather(weather0),explosion(Explosion(world))
{
	SpawnRadius = 15;
	ToBeSpawned = Type::Stone;

	for (size_t i = 0; i < (int)Type::Count ; i++)
	{
		int x = ElemButtonPos.x + (i % Max_rows) * (ElementButton::dim + space);
		int y = ElemButtonPos.y - (i / Max_rows) * (ElementButton::dim + space);
		EButtons.emplace_back(Vec2I{ x , y },Type(i));
	}

	for (size_t i = 0; i < (int)WeatherType::Count; i++)
	{
		int x = WeatherButtonPos.x + i * (ElementButton::dim + space);
		WButtons.emplace_back(Vec2I{ x , WeatherButtonPos.y }, WeatherType(i));
	}

	//elem name list
	ElemName_map.emplace(Type::Empty   , "Empty"   );
	ElemName_map.emplace(Type::Water   , "Water"   );
	ElemName_map.emplace(Type::Sand    , "Sand"    );
	ElemName_map.emplace(Type::Fire    , "Fire"    );
	ElemName_map.emplace(Type::FireAura, "FireAura");
	ElemName_map.emplace(Type::Steam   , "Steam"   );
	ElemName_map.emplace(Type::Snow    , "Snow"    );
	ElemName_map.emplace(Type::Smoke   , "Smoke"   );
	ElemName_map.emplace(Type::ToxicGas, "ToxicGas");
	ElemName_map.emplace(Type::Acid    , "Acid"    );
	ElemName_map.emplace(Type::Wood    , "Wood"    );
	ElemName_map.emplace(Type::Stone   , "Stone"   );

	//weather name list
	WeatherName_map.emplace(WeatherType::Clear , "Clear");
	WeatherName_map.emplace(WeatherType::Rain  , "Rain");
	WeatherName_map.emplace(WeatherType::Snowy , "Snowy");

}


void Creator::Spawn(Mouse& mouse ,MouseLastFrameStats& previous_stats, Sandbox& sandbox ,ParticleEffect& list , Camera& camera)
{
	if (!IsHoveringAButton(mouse) && ToBeSpawned != Type::None)
	{
		if (ToBeSpawned != Type::Explosion)
		{
			if (mouse.LeftIsPressed())
			{
				auto elem_list = GetSpawnableElements(mouse , camera);
				for (auto& n : elem_list)
				{
					if (ToBeSpawned == Type::Water)
					{
						if (Chance.GetVal() > 50)
						{
							if (world.CreateElem(n, ToBeSpawned) == true)
								sandbox.ActivateChunk(int(n));
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
				if (!previous_stats.RightIsPressed())
				{
					SpawnLoc = CoordinateShower::DetermineCoordinates(mouse , camera);
				}
			}
			else if (!mouse.RightIsPressed() && previous_stats.RightIsPressed())
			{
				Vec2I last_pos = CoordinateShower::DetermineCoordinates(previous_stats.GetPos() , camera);
				RectI Zone = RectI(world.ScreenToMatrixPos(SpawnLoc),
					world.ScreenToMatrixPos(last_pos));

				auto dim = world.GetSandboxDim();
				for (int y = Zone.bottom; y != Zone.top(); y += 1)
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
				explosion.ExplodeZone(MousePos , list , ExplosionRadius , DarkeningRadius);
			}

		}
		
	}
}

void Creator::DrawButtons(Graphics& gfx , Camera& cam)
{
	for (size_t i = 0; i < (int)Type::Count; i++)
	{
		EButtons[i].Draw(gfx , cam);
	}
	for (size_t i = 0; i < (int)WeatherType::Count; i++)
	{
		WButtons[i].Draw(gfx , cam);
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


void Creator::ShowHoveredElement(Mouse& mouse, Graphics& gfx , Camera& cam)
{
	Vec2_<int> MousePos = Vec2_<int>( mouse.GetPosX() , mouse.GetPosY() );

	if(Graphics::WithinScreen(MousePos))
	{
		if (!IsHoveringAButton(mouse))
		{
			Vec2_<size_t> WorldPos = Vec2_<size_t>(MousePos.x / World::ElemSize, MousePos.y / World::ElemSize);

			auto dim0 = world.GetSandboxDim();

			const int index = WorldPos.y * dim0.width + WorldPos.x;
			Type type = world.GetElem(world.ScreenToMatrixPos(MousePos))->GetType();

			int Space = 10;

			auto dim1 = font.GetLetterDim();

			Vec2I InfoPos = Vec2I(0, Graphics::ScreenHeight - dim1.height - Space);

			auto& matrix = world.GetMatrix();
			std::vector<int> ElemCounter(int(Type::Count), 0);

			for (const auto& n : matrix)
			{
				int index = int(n.GetType());
				ElemCounter[index]++;
			}

			std::string ElemName = ElemName_map[type];

			const std::string count = std::to_string(ElemCounter[int(type)]);

			InfoPos.x = Graphics::ScreenWidth - ElemName.size() * dim1.width - Space - (count.length() + 2) * dim1.width;
			const std::string result = ElemName + "(" + count + ")";
			font.DrawWords(result, gfx, cam , InfoPos);
		}
		else
		{
			size_t bIndex = -1; // button index
			for (size_t ind = 0; ind < int(Type::Count); ind++)
			{
				if (EButtons[ind].IsHovered(mouse))
				{
					bIndex = ind;
					auto font_dim = font.GetLetterDim();

					const std::string elem_name = ElemName_map[Type(bIndex)];
					const int ElemNameX = (Graphics::ScreenWidth - elem_name.length() * font_dim.width) / 2;

					font.DrawWords(elem_name, gfx, cam , Vec2I(ElemNameX, InfoY));
					break;
				}
			}
			if (bIndex == -1)
			{
				for (size_t ind = 0; ind < int(WeatherType::Count); ind++)
				{
					if (WButtons[ind].IsHovered(mouse))
					{
						bIndex = ind;
						auto font_dim = font.GetLetterDim();

						const std::string weather_name = WeatherName_map[WeatherType(bIndex)];
						const int WeatherNameX = (Graphics::ScreenWidth - weather_name.length() * font_dim.width) / 2;

						font.DrawWords(weather_name, gfx, cam , Vec2I(WeatherNameX, InfoY));
						break;
					}
				}
			}
		}
	
	}
}

void Creator::DrawSpawnSurface(Graphics& gfx , Camera& ct ,  Mouse& mouse)
{

	if (ToBeSpawned != Type::None)
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
			//things are calculated to matrix level and not screen coords
			auto SandboxDim = world.GetSandboxDim();

			auto WorldPos = CoordinateShower::DetermineCoordinates(mouse , ct);
			WorldPos = Vec2I(WorldPos.x / World::ElemSize + SandboxDim.width / 2,
				             WorldPos.y / World::ElemSize + SandboxDim.height / 2);

			int dim = Radius * 2;
			RectI surf = RectI(dim, dim, Vec2I(WorldPos.x - Radius, WorldPos.y - Radius));

			std::vector<size_t> Elements;

			for (int y = surf.bottom; y < surf.top(); y++)
			{
				for (int x = surf.left; x < surf.right(); x++)
				{
					int ind = (y) * SandboxDim.width + x;
					if (ind < SandboxDim.GetArea() && ind >= 0)
					{
						//Vec2I ElemLoc = world.IndexToPos(ind);
						int x = ind % SandboxDim.width, y = ind / SandboxDim.width;
						Vec2I ElemLoc = Vec2I(int(x) ,
											  int(y ));

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
				Vec2I pos = rect.GetPos();
				pos = ct.Transform(pos);
				gfx.DrawRect(RectI(rect.GetDimensions() , std::move(pos)), circle_color, Effects::Copy{});
			}
		}
		else
		{
			RectI Zone = RectI(ct.Transform(CoordinateShower::DetermineCoordinates(mouse , ct))
							 , ct.Transform(SpawnLoc));

			gfx.DrawRect(RectI(10 , 10 , ct.Transform(CoordinateShower::DetermineCoordinates(mouse, ct))), Colors::Green, Effects::Copy{});
			gfx.DrawRect_Border(std::move(Zone), Colors::Green, Effects::Copy{});
		}
	}
	else
	{

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
std::vector<size_t> Creator::GetSpawnableElements(Mouse& mouse , Camera& camera)
{
	auto WorldPos = CoordinateShower::DetermineCoordinates(mouse , camera);
	//converting to matrix coordinates
	WorldPos /= World::ElemSize;
	WorldPos = { WorldPos.x + World::SandboxDim.width / 2 , WorldPos.y + World::SandboxDim.height / 2};

	// // // // //
	int dim = SpawnRadius * 2;
	RectI surf = RectI(dim, dim, Vec2I(WorldPos.x - SpawnRadius, WorldPos.y - SpawnRadius));

	std::vector<size_t> Elements;

	auto SandboxDim = world.GetSandboxDim();
	for (int y = surf.bottom; y < surf.top(); y++)
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

void Creator::ChangeSpawnArea(Mouse& mouse)
{
	auto e = mouse.Read();

	if (e.GetType() == Mouse::Event::Type::WheelUp)
	{
		if (int(ToBeSpawned) < 12) // the types until 12 are ALL regular elements
		{
			SpawnRadius += 0.5f;
		}
		else if (ToBeSpawned == Type::Explosion)
		{
			ExplosionRadius += 0.5f;
			DarkeningRadius = ExplosionRadius / 3.0f;
		}
	}
	else if (e.GetType() == Mouse::Event::Type::WheelDown)
	{
		if (int(ToBeSpawned) < 12) // the types until 12 are ALL regular elements
		{
			SpawnRadius -= 0.5f;
			if (SpawnRadius < MinSpawnRadius)
			{
				SpawnRadius = MinSpawnRadius;
			}
		}
		else if (ToBeSpawned == Type::Explosion)
		{
			ExplosionRadius -= 0.5f;
			if (ExplosionRadius < MinExplosionRadius)
			{
				ExplosionRadius = MinExplosionRadius;
			}
			else
			{
				DarkeningRadius = ExplosionRadius / 3.0f;
			}
		}
	}
}
