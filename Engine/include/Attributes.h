#pragma once
#include "Element.h"

class Attributes
{
public:
	void GetAttributes(const Element& elem)
	{
		type		 = elem.type;
		state		 = elem.state;
		color		 = elem.color;
		BurnChance   = elem.BurnChance;
		BurnDuration = elem.BurnDuration;
		LifeSpan     = elem.LifeSpan;
		ChangeColor  = elem.ChangeColor;
	}
	void PassAttributes(Element& elem) const
	{
		elem.type		  = type;
		elem.state		  = state;
		elem.color		  = color;
		elem.BurnChance   = BurnChance;
		elem.BurnDuration = BurnDuration;
		elem.LifeSpan	  = LifeSpan;
		elem.ChangeColor  = ChangeColor;
	}
public:
	Type type;
	State state;

	Color color;

	unsigned char BurnChance;
	float BurnDuration;

	Countdown LifeSpan;
	Countdown ChangeColor;
};