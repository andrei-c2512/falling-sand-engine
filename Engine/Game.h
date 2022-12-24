/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.h																				  *
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
#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "FrameTimer.h"
#include "Font.h"
#include "World.h"
#include "Creator.h"
#include "Weather.h"
#include <string>
#include "Bench.h"


class Game
{
public:
	Game( class MainWindow& wnd );
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	~Game();
	void Go();
	bool AreButtonsHovered()const;
	void CheckButtons();
private:
	void ComposeFrame();
	void UpdateModel();
	/********************************/
	/*  User Functions              */
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	FrameTimer Timer;
	FrameShower FPS;
	Sprite sprite;
	Sandbox World;
	Creator Builder;
	MouseLastFrameStats MouseStats;
	ParticleEffect particle_list;
	Bench bench;

	Vec2I SpeedButton_pos = Vec2I(Graphics::ScreenWidth - 60 , 60);

	std::vector<GameSpeedButton> button_list;

	float GameSpeed = 1.0f;
	//GameSpeedButton GameSpeed_button = { Colors::Gray , Colors::White ,
	//	RectI(30 , 60 , Vec2I(SpeedButton_pos)) };

	/********************************/
	/*  User Variables              */
	/********************************/
};