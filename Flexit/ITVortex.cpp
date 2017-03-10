#include "ITVortex.h"
#include "global.h"
#include "ITPoint.h"

ITVortex::ITVortex(ITPoint* startPoint, ITPoint* endPoint)
{
	set_StartPoint(startPoint);
	set_EndPoint(endPoint);

	_myLength = get_EndPoint()->distanceFrom(get_StartPoint());
	_myLengthSquared = _myLength * _myLength;

}

void ITVortex::calculateMyLength(void)
{
	set_MyLength(get_EndPoint()->distanceFrom(get_StartPoint()));
}

ITPoint* ITVortex::calcFiniteSvensonAt(ITPoint *targetPoint)
{
	// This method returns a prediction of the velocity (in a ITPoint object) induced at the target point by this vortex.

	/* This function computes the velocity at the point targetPoint induced by this
	* finite straight-line vortex segment using the closed form expression
	* due to Svenson and presented by Sofia Werner in Appendix A of her 2001 thesis.
	* Checked 20120708.
	*/
	float a, b, c, dx, dy, dz, term, sqrtABC, sqrtAterm, a1;

	// a is the square of the distance from the vortex start point to the object (target) point.
	// Since we are usually using a panel mid-point as a target point, the variable a will generally be greater than 0.
	//	a = pow(2, get_StartPoint()->distanceFrom(targetPoint));
	a = (targetPoint->get_X() - get_StartPoint()->get_X())*(targetPoint->get_X() - get_StartPoint()->get_X())
		+ (targetPoint->get_Y() - get_StartPoint()->get_Y())*(targetPoint->get_Y() - get_StartPoint()->get_Y())
		+ (targetPoint->get_Z() - get_StartPoint()->get_Z())*(targetPoint->get_Z() - get_StartPoint()->get_Z());

	b = -2 * (
		(get_EndPoint()->get_X() - get_StartPoint()->get_X())*(targetPoint->get_X() - get_StartPoint()->get_X())
		+ (get_EndPoint()->get_Y() - get_StartPoint()->get_Y())*(targetPoint->get_Y() - get_StartPoint()->get_Y())
		+ (get_EndPoint()->get_Z() - get_StartPoint()->get_Z())*(targetPoint->get_Z() - get_StartPoint()->get_Z())
		);

	c = get_MyLengthSquared();

	dx = (get_EndPoint()->get_Z() - get_StartPoint()->get_Z())*(targetPoint->get_Y() - get_StartPoint()->get_Y())
		- (get_EndPoint()->get_Y() - get_StartPoint()->get_Y())*(targetPoint->get_Z() - get_StartPoint()->get_Z());

	dy = (get_EndPoint()->get_X() - get_StartPoint()->get_X())*(targetPoint->get_Z() - get_StartPoint()->get_Z())
		- (get_EndPoint()->get_Z() - get_StartPoint()->get_Z())*(targetPoint->get_X() - get_StartPoint()->get_X());

	dz = (get_EndPoint()->get_Y() - get_StartPoint()->get_Y())*(targetPoint->get_X() - get_StartPoint()->get_X())
		- (get_EndPoint()->get_X() - get_StartPoint()->get_X())*(targetPoint->get_Y() - get_StartPoint()->get_Y());

	term = 4 * a*c - b*b;

	ITPoint *v = new ITPoint(0.0, 0.0, 0.0);

	if (term == 0.0) // Denominator.
	{
		return v;
	}

	if (a <= 0.0) // Square of a denominator.
	{
		return v;
	}

	if (a + b + c < 0.0) // Square of a denominator.
	{
		return v;
	}

	sqrtABC = sqrt(a + b + c);
	sqrtAterm = b / sqrt(a);
	a1 = -2 * ((2 * c + b) / sqrtABC - sqrtAterm) / term;

	v->set_X(dx * a1);
	v->set_Y(dy * a1);
	v->set_Z(dz * a1);

	return v;
}

ITPoint *ITVortex::get_StartPoint() { return _startPoint; }
void ITVortex::set_StartPoint(ITPoint *p) { _startPoint = p; }

ITPoint *ITVortex::get_EndPoint() { return _endPoint; }
void ITVortex::set_EndPoint(ITPoint *p) { _endPoint = p; }

ITPanel* ITVortex::get_MyITPanel() { return _MyITPanel; }
void ITVortex::set_MyITPanel(ITPanel *p) { _MyITPanel = p; }

float ITVortex::get_MyLength() { return _myLength; }
void ITVortex::set_MyLength(float a) { _myLength = a; }

float ITVortex::get_MyLengthSquared() { return _myLengthSquared; }
void ITVortex::set_MyLengthSquared(float a) { _myLengthSquared = a; }