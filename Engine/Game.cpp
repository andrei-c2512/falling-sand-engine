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
	World(particle_list)
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
	GameSpeed_button.Go(wnd.mouse);
	const float Speed = GameSpeed_button.GetSpeed();
	const float dt = Timer.DeltaTime() * 2;

	particle_list.Update(dt);
	if(GameSpeed_button.IsHovered(wnd.mouse) == false)
		Builder.Spawn(wnd.mouse , MouseStats , World , particle_list);
	Builder.CheckButtons(wnd.mouse);
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
	GameSpeed_button.Draw(gfx);
	/////////////////////////////////////////////////////////
	//gfx.ApplyBloom();

	bench.UploadTime();
	bench.DrawFrameCounter(gfx);

	//bench.End();
	//bench.UploadData();
}
