// System includes.
#include <string>

// Dom's includes.
#include "ITBeamElement.h"
#include "global.h"
#include "ITPoint.h"

ITBeamElement::ITBeamElement()
{

	_startPoint = new ITPoint(0.0, 0.0, 0.0);;
	_endPoint = new ITPoint(0.0, 0.0, 0.0);;

	set_Name("");

}



ITBeamElement::~ITBeamElement(void)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside destructor. ITBeamElement being destroyed.");

	delete _startPoint;
	delete _endPoint;
}


// Utilities.
float ITBeamElement::length()
{

	return get_StartPoint()->distanceFrom(get_EndPoint());

}

// Accessors.
std::string ITBeamElement::get_Name() { return _Name; }
void ITBeamElement::set_Name(std::string s) { _Name = s; }

ITPoint* ITBeamElement::get_StartPoint() { return _startPoint; }
void ITBeamElement::set_StartPoint(ITPoint* p) { _startPoint = p; }

ITPoint* ITBeamElement::get_EndPoint() { return _endPoint; }
void ITBeamElement::set_EndPoint(ITPoint* p) { _endPoint = p; }
