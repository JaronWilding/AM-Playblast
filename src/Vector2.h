#pragma once

class Vector2
{
public:
	double x;
	double y;

	Vector2();
	Vector2(double x, double y);

	// Overloads
	Vector2 operator+ (Vector2 const& obj);
	Vector2 operator- (Vector2 const& obj);
};
