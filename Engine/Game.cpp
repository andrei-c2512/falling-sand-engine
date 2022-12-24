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
	Timer(),
	sprite("dib.bmp"),
	Builder(RectI(10, 10, Vec2I(10, 400)), 0 , World.GetWorld(), World.GetWeather()),
	World(wnd.mouse , particle_list)
{
	//int Space = 10;
	//button_list.emplace_back(Button(RectI(20, 20, SpeedButton_pos) , Sprite("Normal_speed_button.bmp")), 1.0f);
	//button_list.emplace_back(Button(RectI(30, 20, Vec2I(SpeedButton_pos.x , SpeedButton_pos.y + Space + 20)) , 
	//	Sprite("Double_speed_button.bmp")), 1.0f);
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
		Builder.Spawn(wnd.mouse , MouseStats , World , particle_list);
	
	Builder.CheckButtons(wnd.mouse);
	Builder.ChangeSpawnArea(wnd.mouse);
	
	if (!wnd.kbd.KeyIsPressed(VK_SPACE))
	{
		World.UpdateTime(dt);
		World.UpdateSandbox(wnd.mouse , wnd.kbd, dt);
	}
	MouseStats.Update(wnd.mouse);

}

void Game::ComposeFrame()
{
	const float dt = Timer.DeltaTime();
	
	gfx.ResetBloom();
	
	World.DrawSandbox(gfx , wnd.mouse);
	Builder.DrawButtons(gfx);
	Builder.ShowHoveredElement(wnd.mouse, gfx);
	Builder.DrawSpawnSurface(gfx, wnd.mouse);
	
	//FPS.DrawFrameCounter(gfx, dt);
	particle_list.Draw(gfx);
	
	bench.DrawFrameCounter(gfx);
	for (auto& button : button_list)
	{
		button.Draw(gfx);
	}
	/////////////////////////////////////////////////////////
	//gfx.ApplyBloom();
	
	bench.UploadTime();
	bench.DrawFrameCounter(gfx);

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