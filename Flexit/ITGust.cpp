#include "global.h"
#include "ITGust.h"
#include "ITPoint.h"

ITGust::ITGust(float x0, float y0, float z0, float SD, float maxSpeedFactor)
{
	set_X0(x0);
	set_Y0(y0);
	set_Z0(z0);

	set_SD(SD);
	set_MaxSpeed(maxSpeedFactor);
}

// Worker methods.
void ITGust::computeVelocityAtPoint(float x, float y, float z, float *vx, float *vy, float *vz)
{
	*vx = 0.0;
	*vy = 0.0;
	*vz = get_MaxSpeed()*(1.0 / (sqrt(2.0*get_SD()*get_SD()*PI)))*exp(-((x - get_X0())*(x - get_X0())
		+ (y - get_Y0())*(y - get_Y0())
		+ (z - get_Z0())*(z - get_Z0())) / (2.0*get_SD()*get_SD()));
}

float ITGust::get_X0() { return _X0; }
void ITGust::set_X0(float x) { _X0 = x; }

float ITGust::get_Y0() { return _Y0; }
void ITGust::set_Y0(float y) { _Y0 = y; }

float ITGust::get_Z0() { return _Z0; }
void ITGust::set_Z0(float z) { _Z0 = z; }

float ITGust::get_SD() { return _SD; }
void ITGust::set_SD(float sd) { _SD = sd; }

float ITGust::get_MaxSpeed() { return _MaxSpeed; }
void ITGust::set_MaxSpeed(float s) { _MaxSpeed = s; }