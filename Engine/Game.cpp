/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include <sstream>
#include <fstream>
#include <assert.h>
Game::Game(MainWindow& wnd)
	:
	wnd(wnd),
	gfx(wnd),
	ct(gfx),
	cam(ct),
	cs(cam),
	Timer(),
	sprite("dib.bmp"),
	Builder(RectI(10, 10, Vec2I(10, 400)), 0 , World.GetWorld(), World.GetWeather()),
	World(wnd.mouse , particle_list , cam)
{
}

Game::~Game() {
}
void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	bench.Start();

	
	const float dt = Timer.DeltaTime() * GameSpeed;
	
	particle_list.Update(dt);
	
	CheckButtons();

	if(AreButtonsHovered() == false)
		Builder.Spawn(wnd.mouse , MouseStats , World , particle_list , cam);
	
	Builder.CheckButtons(wnd.mouse);
	Builder.ChangeSpawnArea(wnd.mouse);
	
	if (!wnd.kbd.KeyIsPressed(VK_SPACE))
	{
		World.UpdateTime(dt);
		World.UpdateSandbox(wnd.mouse , wnd.kbd, dt);
	}
	MouseStats.Update(wnd.mouse);
	if (wnd.kbd.KeyIsPressed(VK_LEFT))
	{
		cam.MoveLeft(-5);
	}
	else if (wnd.kbd.KeyIsPressed(VK_RIGHT))
	{
		cam.MoveRight(5);
	}

	if (wnd.kbd.KeyIsPressed(VK_UP))
	{
		cam.MoveUp(5);
	}
	else if (wnd.kbd.KeyIsPressed(VK_DOWN))
	{
		cam.MoveDown(-5);
	}

	cam.MouseMovement(wnd.mouse);
	cam.UpdateMovement(dt);
}

void Game::ComposeFrame()
{
	const float dt = Timer.DeltaTime();
	
	gfx.ResetBloom();
	
	World.DrawSandbox(gfx , cam ,  wnd.mouse);
	Builder.DrawButtons(gfx , cam);
	//Builder.ShowHoveredElement(wnd.mouse, gfx, cam);
	Builder.DrawSpawnSurface(gfx, cam , wnd.mouse);
	
	//FPS.DrawFrameCounter(gfx, cam , dt);
	particle_list.Draw(gfx , cam);
	
	for (auto& button : button_list)
	{
		button.Draw(gfx , cam);
	}
	/////////////////////////////////////////////////////////
	//gfx.ApplyBloom();
	cs.ShowCoordinates(gfx, wnd.mouse);
	bench.UploadTime();
	bench.DrawFrameCounter(gfx , cam);
	gfx.DrawRect_Border(Graphics::WorldArea, Colors::Magenta, Effects::Copy{}, Graphics::GetScreenRect());
	//if (wnd.mouse.LeftIsPressed())
	//{
	//	auto list = AStarAlgorithm::ApplyAlgorithm(Vec2I(200, 200), wnd.mouse.GetPos(),
	//		World.GetWorld(), wnd.mouse , gfx ,  Graphics::GetScreenRect());
	//
	//	//gfx.DrawOpenPoly(list);
	//}
}

void Game::CheckButtons() 
{
	for (auto& button : button_list)
	{
		if (button.IsPressed(wnd.mouse))
		{
			GameSpeed = button.GetSpeed();
		}
	}
}

bool Game::AreButtonsHovered() const
{
	for (auto& button : button_list)
	{
		if (button.IsHovered(wnd.mouse))
		{
			return true;
		}
	}
	return false;
}