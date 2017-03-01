// Dom's includes.
#include "ITControlPoint.h"
#include "global.h"
#include "ITProject.h"

ITControlPoint::ITControlPoint(float x, float y, float z) : ITPoint(x, y, z)
{
	// Set all the initial deflections to zero.
	_W = new ITPoint(0.0, 0.0, 0.0);

}


ITControlPoint::~ITControlPoint(void)
{
	delete _W;
}


// Accessors
ITPoint *ITControlPoint::get_W() { return _W; }
void ITControlPoint::set_W(ITPoint *w) { _W = w; }
