#include "Vector2.h"

// Constructors

Vector2::Vector2()
{
	this->x = 0;
	this->y = 0;
}

Vector2::Vector2(double x, double y)
{
	this->x = x;
	this->y = y;
}

Vector2::Vector2(MTime x, MTime y)
{
	this->mX = x;
	this->mY = y;
	this->x = x.value();
	this->y = y.value();
}


Vector2 Vector2::operator+ (Vector2 const& obj)
{
	return Vector2(x + obj.x, y + obj.y);
}

Vector2 Vector2::operator- (Vector2 const& obj)
{
	return Vector2(x - obj.x, y - obj.y);
}