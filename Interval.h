#pragma once
#include "Helpers.h"

class Interval
{
public:
	float minimum, maximum;

	Interval() : minimum(+infinity), maximum(-infinity) {} // Default interval is empty
	Interval(float _minimum, float _maximum) : minimum(_minimum), maximum(_maximum) {}

	float Size() const;
	bool Contains(float _x) const;
	bool Surrounds(float _x) const;
	float Clamp(float _x) const;

	static const Interval Empty, Universe;
};

