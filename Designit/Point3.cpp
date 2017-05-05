#include "Point3.h"

#include "global.h"

Point3::Point3() : x(0), y(0), z(0) {}

Point3::Point3(const float x, const float y, const float z) : x(x), y(y), z(z) {}

Point3 Point3::operator+(const Point3 other)
{
	Point3 p;
	p.x = this->x + other.x;
	p.y = this->y + other.y;
	p.z = this->z + other.z;
	return p;
}

Point3 Point3::operator-(const Point3 other)
{
	Point3 p;
	p.x = this->x - other.x;
	p.y = this->y - other.y;
	p.z = this->z - other.z;
	return p;
}

Point3 Point3::operator*(const float n)
{
	Point3 p;
	p.x = this->x * n;
	p.y = this->y * n;
	p.z = this->z * n;
	return p;
}

Point3 Point3::operator/(const float n)
{
	Point3 p;
	p.x = this->x / n;
	p.y = this->y / n;
	p.z = this->z / n;
	return p;
}

void Point3::normalize()
{
	float length = sqrtf(x*x + y*y + z*z);

	x /= length;
	y /= length;
	z /= length;
}

void Point3::toCartesian(const float radius, const float angle, const PLANE _plane)
{
	switch (_plane)
	{
	case XY:
		x = radius * cosf(angle);
		y = radius * sinf(angle);
		z = 0.0;
		break;
	case XZ:
		x = radius * cosf(angle);
		y = 0.0;
		z = radius * sinf(angle);
		break;
	case YZ:
		x = 0.0;
		y = radius * cosf(angle);
		z = radius * sinf(angle);
		break;
	}
}

void Point3::reset()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
}
