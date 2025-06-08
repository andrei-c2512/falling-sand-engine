#pragma once
#include "Graphics.h"
#include "Camera.h"

class ElementDrawer {
public:
	static void drawDefault(Graphics& gfx, const Camera& ct);
	static void drawGas(Graphics& gfx, const Camera& ct);
	static void drawWater(Graphics& gfx, const Camera& ct);
	static void drawPlasmaOrAcid(Graphics& gfx, const Camera& ct);
};