#pragma once

#include <maya/MAnimControl.h>

class Vector2
{
public:
	double x;
	double y;
	MTime mX;
	MTime mY;

	Vector2();
	Vector2(double x, double y);
	Vector2(MTime x, MTime y);

	// Overloads
	Vector2 operator+ (Vector2 const& obj);
	Vector2 operator- (Vector2 const& obj);
};
