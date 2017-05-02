#include "ITPoint.h"
#include "ITSurface.h"
#include "global.h"
#include "UtililityFunctions.h"

ITPoint::ITPoint(float x, float y, float z)
{
	set_X(x);
	set_Y(y);
	set_Z(z);

	set_VX(0.0f);
	set_VY(0.0f);
	set_VZ(0.0f);

	set_Mass(0.0);
}

ITPoint::ITPoint( ITPoint& p )
{
	set_X(p.get_X());
	set_Y(p.get_Y());
	set_Z(p.get_Z());

	set_VX(p.get_VX());
	set_VY(p.get_VY());
	set_VZ(p.get_VZ());

	set_Mass(p.get_Mass());
}

ITPoint::~ITPoint(void)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside destructor. ITPoint being destroyed.");
}

float ITPoint::magnitude()
{
	return sqrt(get_X()*get_X() + get_Y()*get_Y() + get_Z()*get_Z());
}

void ITPoint::normalize()
{
	float tempMag = magnitude();

	set_X(get_X() / tempMag);
	set_Y(get_Y() / tempMag);
	set_Z(get_Z() / tempMag);
}

ITPoint* ITPoint::cross(ITPoint* secondPoint)
{
	ITPoint* resultant = new ITPoint(get_Y()*secondPoint->get_Z() - get_Z()*secondPoint->get_Y(),
		get_Z()*secondPoint->get_X() - get_X()*secondPoint->get_Z(),
		get_X()*secondPoint->get_Y() - get_Y()*secondPoint->get_X());

	return resultant;
}

float ITPoint::dot(ITPoint* secondPoint)
{
	return this->get_X() * secondPoint->get_X()
		+ this->get_Y() * secondPoint->get_Y()
		+ this->get_Z() * secondPoint->get_Z();
}

float ITPoint::distanceFrom(ITPoint* secondPoint)
{
	return sqrt(
		(this->get_X() - secondPoint->get_X())*(this->get_X() - secondPoint->get_X())
		+ (this->get_Y() - secondPoint->get_Y())*(this->get_Y() - secondPoint->get_Y())
		+ (this->get_Z() - secondPoint->get_Z())*(this->get_Z() - secondPoint->get_Z())
	);
}

QString ITPoint::getAnnotationText()
{
	return QString("k: %1, i: %2, j: %3, \r \n x: %4, y: %5, z: %6").arg(_k).arg(_i).arg(_j).arg(_X).arg(_Y).arg(_Z);
}

void ITPoint::propagateMe(ITPoint* cp, ITPoint* rotationPoint, ITPoint* translationPoint)
{
	// Moves a temporary point from the base position during FlowitCudaUnsteady play out.

	// cp is the surface "datum" point about which rotations should take place.
	// rotationPoint is the point in roll-pitch-yaw space representing the required rotation.
	// translationPoint is the point in translation space representing the required translation.

	// This method operates on the coordinates of this ITPoint object in place.
	float x, y, z;

	// Now do rigid body transformation.
	// Translate this point back to the datum point (in preparation for rotation).
	set_X(get_X() - cp->get_X());
	set_Y(get_Y() - cp->get_Y());
	set_Z(get_Z() - cp->get_Z());

	// Now rotate - Roll first.
	float rollAngleRad = rotationPoint->get_X();

	y = get_Y()*cos(rollAngleRad) - get_Z()*sin(rollAngleRad);
	z = get_Y()*sin(rollAngleRad) + get_Z()*cos(rollAngleRad);

	set_Y(y);
	set_Z(z);

	// Now rotate - Pitch second.
	float pitchAngleRad = -rotationPoint->get_Y();

	x = get_X()*cos(pitchAngleRad) - get_Z()*sin(pitchAngleRad);
	z = get_X()*sin(pitchAngleRad) + get_Z()*cos(pitchAngleRad);

	set_X(x);
	set_Z(z);

	// Now rotate - Yaw third.
	float yawAngleRad = rotationPoint->get_Z();

	x = get_X()*cos(yawAngleRad) - get_Y()*sin(yawAngleRad);
	y = get_X()*sin(yawAngleRad) + get_Y()*cos(yawAngleRad);

	set_X(x);
	set_Y(y);

	// Next, translate to the new location (includes undoing of the translation prior to rotation above).
	set_X(get_X() + cp->get_X() + translationPoint->get_X());
	set_Y(get_Y() + cp->get_Y() + translationPoint->get_Y());
	set_Z(get_Z() + cp->get_Z() + translationPoint->get_Z());
}

void ITPoint::rotateAround(ITPoint& center, const float angle, PLANE p)
{
	float pxOld = get_X() - center.get_X();
	float pyOld = get_Y() - center.get_Y();
	float pzOld = get_Z() - center.get_Z();

	float pxNew, pyNew, pzNew;

	switch (p) 
	{
	case XY:
		pxNew = pxOld*cos(angle) - pyOld*sin(angle);
		pyNew = pxOld*sin(angle) + pyOld*cos(angle);
		pzNew = pzOld;
		break;
	case XZ:
		pxNew = pxOld*cos(angle) - pzOld*sin(angle);
		pyNew = pyOld;
		pzNew = pxOld*sin(angle) + pzOld*cos(angle);
		break;
	case YZ:
		pxNew = pxOld;
		pyNew = pyOld*cos(angle) - pzOld*sin(angle);
		pzNew = pyOld*sin(angle) + pzOld*cos(angle);
		break;
	}

	pxNew += center.get_X();
	pyNew += center.get_Y();
	pzNew += center.get_Z();

	set_X(pxNew);
	set_Y(pyNew);
	set_Z(pzNew);
}

Point3 ITPoint::getCoordinates()
{
	return Point3(_X, _Y, _Z);
}

// Accessors.
float ITPoint::get_X() { return _X; }
void ITPoint::set_X(float x) { _X = x; }

float ITPoint::get_Y() { return _Y; }
void ITPoint::set_Y(float y) { _Y = y; }

float ITPoint::get_Z() { return _Z; }
void ITPoint::set_Z(float z) { _Z = z; }

float ITPoint::get_VX() { return _VX; }
void ITPoint::set_VX(float vx) { _VX = vx; }

float ITPoint::get_VY() { return _VY; }
void ITPoint::set_VY(float vy) { _VY = vy; }

float ITPoint::get_VZ() { return _VZ; }
void ITPoint::set_VZ(float vz) { _VZ = vz; }

float ITPoint::get_Mass() { return _Mass; }
void ITPoint::set_Mass(float m) { _Mass = m; }

int ITPoint::get_I() { return _i; }
void ITPoint::set_I(int i) { _i = i; }

int ITPoint::get_J() { return _j; }
void ITPoint::set_J(int j) { _j = j; }

int ITPoint::get_K() { return _k; }
void ITPoint::set_K(int k) { _k = k; }

float ITPoint::get_U() { return _U; }
void ITPoint::set_U(float u) { _U = u; }

float ITPoint::get_V() { return _V; }
void ITPoint::set_V(float v) { _V = v; }