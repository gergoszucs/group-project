#pragma once

enum PLANE;

class Point3
{
public:
	float x, y, z;

	Point3();

	Point3(const float x, const float y, const float z);

	Point3 operator+(const Point3 other);
	Point3 operator-(const Point3 other);
	Point3 operator*(const float n);
	Point3 operator/(const float n);

	void normalize();
	void toCartesian(const float radius, const float angle, const PLANE _plane);

	void reset();
};