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

#include "chili/Keyboard.hpp"
#include "chili/Mouse.hpp"
#include "engine2d/core/Graphics.hpp"
#include "engine2d/FrameTimer.hpp"
#include "falling-sand/World.hpp"
#include "falling-sand/ui/Creator.hpp"
#include "falling-sand/Weather.hpp"
#include "falling-sand/Sandbox.hpp"
#include "falling-sand/ParticleEffect.hpp"
#include "engine2d/ui/Bench.hpp"


namespace chili {
	class Game
	{
	public:
		Game(class MainWindow& wnd);
		Game(const Game&) = delete;
		Game& operator=(const Game&) = delete;
		~Game();
		void Go();
	private:
		void ComposeFrame();
		void UpdateModel();
		/********************************/
		/*  User Functions              */
		/********************************/
	private:
		MainWindow& wnd;

		e2d::Renderer gfx;
		e2d::CoordinateTransformer ct;
		e2d::CoordinateShower cs;
		e2d::Camera cam;

		e2d::FrameTimer Timer;
		e2d::FrameShower FPS;
		sand::Sandbox World;
		sand::Creator Builder;
		chili::MouseLastFrameStats MouseStats;
		sand::ParticleEffect particle_list;
		e2d::Bench bench;

		e2d::Vec2I SpeedButton_pos = e2d::Vec2I(e2d::Renderer::ScreenWidth - 60, 60);

		//std::vector<sand::GameSpeedButton> button_list;

		float GameSpeed = 3.0f;
		//GameSpeedButton GameSpeed_button = { Colors::Gray , Colors::White ,
		//	RectI(30 , 60 , Vec2I(SpeedButton_pos)) };

		/********************************/
		/*  User Variables              */
		/********************************/
	};
}