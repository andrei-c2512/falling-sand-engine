#pragma once
#include <concepts>
#include "chili/Colors.hpp"


/*
	Here I define the rendering API used within this project
*/

typedef int16_t TextureID;
typedef int16_t FontID;

template<typename T>
concept GraphicsAPI = requires(T g, int x, int y, int width, int height, int32_t color, 
							   TextureID texture, FontID font, const char* text,
							   int index) {
	{ g.PutPixel(x, y, color) } -> std::same_as<void>;
	{ g.PutPixel(index, color) } -> std::same_as<void>;
	{ g.DrawRect(x, y, width, height, color) } -> std::same_as<void>; 
	{ g.DrawRectClipped(x, y, width, height, color) } -> std::same_as<void>;
	/* This is for later, as I have not yet set up a asset loader */
	// { g.DrawSprite(x, y, texture) } -> std::same_as<void>;
	// { g.DrawText(x, y, text, font) } -> std::same_as<void>;
	/**************************************************************/
};
