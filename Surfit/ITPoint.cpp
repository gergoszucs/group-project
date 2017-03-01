// System includes.
#include <string>

// Dom's includes.
#include "ITPoint.h"
#include "ITSurface.h"
#include "global.h"


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

ITPoint::ITPoint()
{
}


ITPoint::~ITPoint(void)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside destructor. ITPoint being destroyed.");
}

float ITPoint::magnitude()
{
	return sqrt(get_X()*get_X() + get_Y()*get_Y() + get_Z()*get_Z() );
}	

void ITPoint::normalize()
{

	float tempMag = magnitude();

	set_X( get_X()/tempMag );
	set_Y( get_Y()/tempMag );
	set_Z( get_Z()/tempMag );	
}


ITPoint* ITPoint::cross(ITPoint* secondPoint)
{
	ITPoint* resultant = new ITPoint( get_Y()*secondPoint->get_Z() - get_Z()*secondPoint->get_Y(), 
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
} // End of distanceFrom.



// Accessors.
float ITPoint::get_X(){ return _X;}
void ITPoint::set_X(float x){_X = x;}

float ITPoint::get_Y(){	return _Y;}
void ITPoint::set_Y(float y){_Y = y;}

float ITPoint::get_Z(){	return _Z;}
void ITPoint::set_Z(float z){_Z = z;}

float ITPoint::get_VX(){ return _VX;}
void ITPoint::set_VX(float vx){_VX = vx;}

float ITPoint::get_VY(){ return _VY;}
void ITPoint::set_VY(float vy){_VY = vy;}

float ITPoint::get_VZ(){ return _VZ;}
void ITPoint::set_VZ(float vz){_VZ = vz;}

float ITPoint::get_Mass(){ return _Mass; }
void ITPoint::set_Mass(float m){ _Mass = m; }

int ITPoint::get_I(){ return _i; }
void ITPoint::set_I(int i){ _i = i; }

int ITPoint::get_J(){ return _j; }
void ITPoint::set_J(int j){ _j = j; }

int ITPoint::get_K(){ return _k; }
void ITPoint::set_K(int k){ _k = k; }

float ITPoint::get_U(){ return _U; }
void ITPoint::set_U(float u){ _U = u; }

float ITPoint::get_V(){ return _V; }
void ITPoint::set_V(float v){ _V = v; }
