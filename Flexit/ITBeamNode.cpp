// Dom's includes.
#include "ITBeamNode.h"
#include "global.h"
#include "ITProject.h"

ITBeamNode::ITBeamNode(float x, float y, float z) : ITPoint(x, y, z)
{
	// Set all the initial deflections to zero.
	_W = new ITPoint(0.0, 0.0, 0.0);
	_Wnext = new ITPoint(0.0, 0.0, 0.0);
	_Wlast = new ITPoint(0.0, 0.0, 0.0);

	// Initialize the applied load to zero.
	_q = new ITPoint(0.0, 0.0, 0.0);

	_Mass = 0.0;
	_MassPerUnitLength = 0.0;
	_RotationAngle = 0;
}

ITBeamNode::~ITBeamNode(void)
{
	//	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside destructor. ITBeamNode being destroyed.");
	delete _W;
	delete _Wnext;
	delete _Wlast;
	delete _q;
}

// Accessors
ITPoint *ITBeamNode::get_W() { return _W; }
void ITBeamNode::set_W(ITPoint *w) { _W = w; }

ITPoint *ITBeamNode::get_Wnext() { return _Wnext; }
void ITBeamNode::set_Wnext(ITPoint *w) { _Wnext = w; }

ITPoint *ITBeamNode::get_Wlast() { return _Wlast; }
void ITBeamNode::set_Wlast(ITPoint *w) { _Wlast = w; }

ITPoint *ITBeamNode::get_q() { return _q; }
void ITBeamNode::set_q(ITPoint *q) { _q = q; }

float ITBeamNode::get_Mass() { return _Mass; }
void ITBeamNode::set_Mass(float m) { _Mass = m; }

float ITBeamNode::get_MassPerUnitLength() { return _MassPerUnitLength; }
void ITBeamNode::set_MassPerUnitLength(float m) { _MassPerUnitLength = m; }

float ITBeamNode::get_E() { return _E; }
void ITBeamNode::set_E(float e) { _E = e; }

float ITBeamNode::get_Ix() { return _Ix; }
void ITBeamNode::set_Ix(float i) { _Ix = i; }

float ITBeamNode::get_Iz() { return _Iz; }
void ITBeamNode::set_Iz(float i) { _Iz = i; }

float ITBeamNode::get_RotationAngle() { return _RotationAngle; }
void ITBeamNode::set_RotationAngle(float r) { _RotationAngle = r; }