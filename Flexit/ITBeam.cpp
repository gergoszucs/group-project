// System includes.
#include <string>

// Dom's includes.
#include "ITBeam.h"
#include "global.h"
#include "ITSurface.h"
#include "ITBeamElement.h"
#include "ITBeamNode.h"
#include "ITPoint.h"
#include "ITControlPoint.h"

ITBeam::ITBeam(ITSurface *s)
{

	// Set my parent ITSurface.
	set_MyITSurface(s);

	// Create the beam elements.
	_MyBeamElements = new std::vector <ITBeamElement *>;

	// Create the beam nodes.
	_MyBeamNodes = new std::vector <ITBeamNode *>;

	// Create the ghost nodes.
	_MyLeftGhostBeamNodes = new std::vector <ITBeamNode *>;
	_MyRightGhostBeamNodes = new std::vector <ITBeamNode *>;

	// Create the ghost elements.
	_MyLeftGhostBeamElements = new std::vector <ITBeamElement *>;
	_MyRightGhostBeamElements = new std::vector <ITBeamElement *>;

	// Create the node history.
	_MyNodeHistory = new std::vector < std::vector <ITBeamNode*> >;
}



ITBeam::~ITBeam(void)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside destructor. ITBeam being destroyed.");

	// Delete beam elements.
	int noOfElements = get_MyBeamElements()->size();
	for (int i = 0; i<noOfElements; i++)
	{
		delete get_MyBeamElements()->at(i);
	}

	get_MyBeamElements()->clear();
	delete _MyBeamElements;

	// Delete beam nodes.
	int noOfNodes = get_MyBeamNodes()->size();
	for (int i = 0; i<noOfNodes; i++)
	{
		delete get_MyBeamNodes()->at(i);
	}

	get_MyBeamNodes()->clear();
	delete _MyBeamNodes;

	// Delete ghost beam nodes.
	noOfNodes = get_MyLeftGhostBeamNodes()->size();
	for (int i = 0; i<noOfNodes; i++)
	{
		delete get_MyLeftGhostBeamNodes()->at(i);
	}

	get_MyLeftGhostBeamNodes()->clear();
	delete _MyLeftGhostBeamNodes;

	noOfNodes = get_MyRightGhostBeamNodes()->size();
	for (int i = 0; i<noOfNodes; i++)
	{
		delete get_MyRightGhostBeamNodes()->at(i);
	}

	get_MyRightGhostBeamNodes()->clear();
	delete _MyRightGhostBeamNodes;


	// Delete the node history.
	int noOfGens = get_MyNodeHistory()->size();
	for (int i = 0; i<noOfGens; i++)
	{
		int noOfNodes = get_MyNodeHistory()->at(i).size();
		for (int j = 0; j<noOfNodes; j++)
		{
			delete get_MyNodeHistory()->at(i).at(j);
		}
		get_MyNodeHistory()->at(i).clear();
	}

	get_MyNodeHistory()->clear();
	delete _MyNodeHistory;

}




void ITBeam::createBeamElements()
{
	int NO_OF_ELEMENTS = get_MyITSurface()->get_MyPanels()->size();

	int colIndex = ceil(get_MyITSurface()->get_MyControlPoints()->at(0).size() / 2);

	// Beam start point.
	float xs = get_MyITSurface()->get_MyControlPoints()->at(0).at(colIndex)->get_X();
	float ys = get_MyITSurface()->get_MyControlPoints()->at(0).at(colIndex)->get_Y();
	float zs = get_MyITSurface()->get_MyControlPoints()->at(0).at(colIndex)->get_Z();

	// Beam end point.
	float xe = get_MyITSurface()->get_MyControlPoints()->back().at(colIndex)->get_X();
	float ye = get_MyITSurface()->get_MyControlPoints()->back().at(colIndex)->get_Y();
	float ze = get_MyITSurface()->get_MyControlPoints()->back().at(colIndex)->get_Z();

	float dx = (xe - xs) / (NO_OF_ELEMENTS + 1);
	float dy = (ye - ys) / (NO_OF_ELEMENTS + 1);
	float dz = (ze - zs) / (NO_OF_ELEMENTS + 1);

	for (int i = 0; i<NO_OF_ELEMENTS; i++)
	{
		ITBeamElement *e = new ITBeamElement();
		e->get_StartPoint()->set_X(xs + dx*i + dx / 2);
		e->get_StartPoint()->set_Y(ys + dy*i + dy / 2);
		e->get_StartPoint()->set_Z(zs + dz*i + dz / 2);

		e->get_EndPoint()->set_X(xs + dx*(i + 1) + dx / 2);
		e->get_EndPoint()->set_Y(ys + dy*(i + 1) + dy / 2);
		e->get_EndPoint()->set_Z(zs + dz*(i + 1) + dz / 2);

		get_MyBeamElements()->push_back(e);
	}

} // End of createBeamElements.




void ITBeam::createGhostBeamElements()
{

	int NO_OF_ELEMENTS = 4;

	// Create ghost elements.

	// Left.
	float dxl = get_MyBeamElements()->at(0)->get_StartPoint()->get_X() - get_MyBeamElements()->at(0)->get_EndPoint()->get_X();
	float dyl = get_MyBeamElements()->at(0)->get_StartPoint()->get_Y() - get_MyBeamElements()->at(0)->get_EndPoint()->get_Y();
	float dzl = get_MyBeamElements()->at(0)->get_StartPoint()->get_Z() - get_MyBeamElements()->at(0)->get_EndPoint()->get_Z();

	ITPoint *p = get_MyBeamElements()->at(0)->get_StartPoint();

	for (int i = 0; i<NO_OF_ELEMENTS; i++)
	{

		ITBeamElement *e = new ITBeamElement();

		e->get_StartPoint()->set_X(p->get_X() + (i + 1)*dxl);
		e->get_StartPoint()->set_Y(p->get_Y() + (i + 1)*dyl);
		e->get_StartPoint()->set_Z(p->get_Z() + (i + 1)*dzl);

		e->get_EndPoint()->set_X(p->get_X() + i*dxl);
		e->get_EndPoint()->set_Y(p->get_Y() + i*dyl);
		e->get_EndPoint()->set_Z(p->get_Z() + i*dzl);

		get_MyLeftGhostBeamElements()->push_back(e);

	}

	// Right.
	float dxr = get_MyBeamElements()->back()->get_EndPoint()->get_X() - get_MyBeamElements()->back()->get_StartPoint()->get_X();
	float dyr = get_MyBeamElements()->back()->get_EndPoint()->get_Y() - get_MyBeamElements()->back()->get_StartPoint()->get_Y();
	float dzr = get_MyBeamElements()->back()->get_EndPoint()->get_Z() - get_MyBeamElements()->back()->get_StartPoint()->get_Z();

	p = get_MyBeamElements()->back()->get_EndPoint();

	for (int i = 0; i<NO_OF_ELEMENTS; i++)
	{

		ITBeamElement *e = new ITBeamElement();

		e->get_StartPoint()->set_X(p->get_X() + i*dxr);
		e->get_StartPoint()->set_Y(p->get_Y() + i*dyr);
		e->get_StartPoint()->set_Z(p->get_Z() + i*dzr);

		e->get_EndPoint()->set_X(p->get_X() + (i + 1)*dxr);
		e->get_EndPoint()->set_Y(p->get_Y() + (i + 1)*dyr);
		e->get_EndPoint()->set_Z(p->get_Z() + (i + 1)*dzr);

		get_MyRightGhostBeamElements()->push_back(e);

	}

} // End of createGhostBeamElements.



void ITBeam::propagateBeamElements()
{
	// This method moves the undeflected beam elements so they match up with the UVLM panels at this Frame.
	// The method assumes the (undeflected) beam is a simple straight line from wing tip to wing tip.

	//	int NO_OF_ELEMENTS = get_MyITSurface()->get_MyInterpolatedPoints()->size() - 2;
	int NO_OF_ELEMENTS = get_MyITSurface()->get_MyBeam()->get_MyBeamElements()->size();

	int colIndex = ceil(get_MyITSurface()->get_MyControlPoints()->at(0).size() / 2);

	float xs = get_MyITSurface()->get_MyControlPoints()->at(0).at(colIndex)->get_X();
	float ys = get_MyITSurface()->get_MyControlPoints()->at(0).at(colIndex)->get_Y();
	float zs = get_MyITSurface()->get_MyControlPoints()->at(0).at(colIndex)->get_Z();

	float xe = get_MyITSurface()->get_MyControlPoints()->back().at(colIndex)->get_X();
	float ye = get_MyITSurface()->get_MyControlPoints()->back().at(colIndex)->get_Y();
	float ze = get_MyITSurface()->get_MyControlPoints()->back().at(colIndex)->get_Z();

	float dx = (xe - xs) / (NO_OF_ELEMENTS + 1);
	float dy = (ye - ys) / (NO_OF_ELEMENTS + 1);
	float dz = (ze - zs) / (NO_OF_ELEMENTS + 1);

	for (int i = 0; i<NO_OF_ELEMENTS; i++)
	{
		ITBeamElement *e = get_MyBeamElements()->at(i);
		e->get_StartPoint()->set_X(xs + dx*i + dx / 2);
		e->get_StartPoint()->set_Y(ys + dy*i + dy / 2);
		e->get_StartPoint()->set_Z(zs + dz*i + dz / 2);

		e->get_EndPoint()->set_X(xs + dx*(i + 1) + dx / 2);
		e->get_EndPoint()->set_Y(ys + dy*(i + 1) + dy / 2);
		e->get_EndPoint()->set_Z(zs + dz*(i + 1) + dz / 2);
	}

} // End of propagateBeamElements.



void ITBeam::propagateGhostBeamElements()
{

	// This method moves the undeflected beam ghost elements so they match up with the UVLM panels at this Frame.
	// The method assumes the (undeflected) ghost beam elements extend along the straight line from wing tip to wing tip.

	int NO_OF_ELEMENTS = 4;

	// Left.
	float dxl = get_MyBeamElements()->at(0)->get_StartPoint()->get_X() - get_MyBeamElements()->at(0)->get_EndPoint()->get_X();
	float dyl = get_MyBeamElements()->at(0)->get_StartPoint()->get_Y() - get_MyBeamElements()->at(0)->get_EndPoint()->get_Y();
	float dzl = get_MyBeamElements()->at(0)->get_StartPoint()->get_Z() - get_MyBeamElements()->at(0)->get_EndPoint()->get_Z();


	ITPoint *p = get_MyBeamElements()->at(0)->get_StartPoint();

	for (int i = 0; i<NO_OF_ELEMENTS; i++)
	{

		ITBeamElement *e = get_MyLeftGhostBeamElements()->at(i);

		e->get_StartPoint()->set_X(p->get_X() + (i + 1)*dxl);
		e->get_StartPoint()->set_Y(p->get_Y() + (i + 1)*dyl);
		e->get_StartPoint()->set_Z(p->get_Z() + (i + 1)*dzl);

		e->get_EndPoint()->set_X(p->get_X() + i*dxl);
		e->get_EndPoint()->set_Y(p->get_Y() + i*dyl);
		e->get_EndPoint()->set_Z(p->get_Z() + i*dzl);

	}

	// Right.
	float dxr = get_MyBeamElements()->back()->get_EndPoint()->get_X() - get_MyBeamElements()->back()->get_StartPoint()->get_X();
	float dyr = get_MyBeamElements()->back()->get_EndPoint()->get_Y() - get_MyBeamElements()->back()->get_StartPoint()->get_Y();
	float dzr = get_MyBeamElements()->back()->get_EndPoint()->get_Z() - get_MyBeamElements()->back()->get_StartPoint()->get_Z();

	ITPoint *pp = get_MyBeamElements()->back()->get_EndPoint();

	for (int i = 0; i<NO_OF_ELEMENTS; i++)
	{

		ITBeamElement *e = get_MyRightGhostBeamElements()->at(i);

		e->get_StartPoint()->set_X(pp->get_X() + i*dxr);
		e->get_StartPoint()->set_Y(pp->get_Y() + i*dyr);
		e->get_StartPoint()->set_Z(pp->get_Z() + i*dzr);

		e->get_EndPoint()->set_X(pp->get_X() + (i + 1)*dxr);
		e->get_EndPoint()->set_Y(pp->get_Y() + (i + 1)*dyr);
		e->get_EndPoint()->set_Z(pp->get_Z() + (i + 1)*dzr);

	}

} // End of propagateGhostBeamElements.







void ITBeam::createBeamNodes()
{
	// NO_OF_NODES is designed to be the same as the number of panels.
	// This is assumed to be ODD.
	int NO_OF_NODES = get_MyITSurface()->get_MyPanels()->size();

	int colIndex = floor(get_MyITSurface()->get_MyControlPoints()->at(0).size() / 2);

	// Beam start point.
	float xs = get_MyITSurface()->get_MyControlPoints()->at(0).at(colIndex)->get_X();
	float ys = get_MyITSurface()->get_MyControlPoints()->at(0).at(colIndex)->get_Y();
	float zs = get_MyITSurface()->get_MyControlPoints()->at(0).at(colIndex)->get_Z();

	// Beam end point.
	float xe = get_MyITSurface()->get_MyControlPoints()->back().at(colIndex)->get_X();
	float ye = get_MyITSurface()->get_MyControlPoints()->back().at(colIndex)->get_Y();
	float ze = get_MyITSurface()->get_MyControlPoints()->back().at(colIndex)->get_Z();

	float dx = (xe - xs) / (NO_OF_NODES);
	float dy = (ye - ys) / (NO_OF_NODES);
	float dz = (ze - zs) / (NO_OF_NODES);

	for (int i = 0; i<NO_OF_NODES; i++)
	{
		ITBeamNode *n = new ITBeamNode(xs + dx*i + dx / 2,
			ys + dy*i + dy / 2,
			zs + dz*i + dz / 2);


		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "======================== Coordinates: %f, %f, %f", n->get_X(), n->get_Y(), n->get_Z());



		// FIXME: This is a temporary fix. In the longer term, beam node masses (and geometry) should be read from the JSON input file.
		//		float m = 0.1 + 0.2*sin(PI*i/(NO_OF_NODES-1));
		//		float m = 26000.0; // Kg for AX-1.

		//		float m = 1020732.0;  // AX-1 20170105.
		float m = 1020732.0;  // AX-1 20170105a.


		int midPointNodeIndex = floor(NO_OF_NODES / 2);

		// Triangular distribution.
		if (i<midPointNodeIndex)
		{
			n->set_Mass(((i + 1)*2.0 / NO_OF_NODES) * m / NO_OF_NODES);
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "i: %i, mass: %f", i, n->get_Mass());
		}
		else
		{
			n->set_Mass(((NO_OF_NODES - i)*2.0 / NO_OF_NODES) * m / NO_OF_NODES);
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "i: %i, mass: %f", i, n->get_Mass());
		}

		float span = fabs(ye - ys);

		// 20170120: The following line is likely wrong as the denominator should be the length of the beam element and not the entire span of the beam.
		//		n->set_MassPerUnitLength(m / span);

		// 20170120: The following line of code is a temporary fix for the AX1i_100_frames_no_duplicates.json file.
		n->set_MassPerUnitLength(m / 1.4);

		//int index = (int)NO_OF_NODES / 2;
		//if (i == index)
		//{
		//	n->set_Mass(0.0008);
		//}

		// FIXME: Temporary assignment of hard-coded E and Ix. This will be read from JSON.
		n->set_E(70.0e9); // Aerospace grade aluminium alloy.
						  //		n->set_E( 3.4e9 ); // Glass fibre.

		float B = 1.0;
		//		float H = 0.0052;
		//		float H = 0.393; // 20170117
		float H = 0.3928; // 20170117a

		n->set_Ix((B*H*H*H) / 12.0);
		//		n->set_Iz( 0.0 );

		get_MyBeamNodes()->push_back(n);
	}

} // End of createBeamNodes.


void ITBeam::createGhostBeamNodes()
{

	// Create ghost nodes.

	// Left.
	float dxl = get_MyBeamNodes()->at(0)->get_X() - get_MyBeamNodes()->at(1)->get_X();
	float dyl = get_MyBeamNodes()->at(0)->get_Y() - get_MyBeamNodes()->at(1)->get_Y();
	float dzl = get_MyBeamNodes()->at(0)->get_Z() - get_MyBeamNodes()->at(1)->get_Z();

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside createGhostBeamNodes. dxl: %f, dyl: %f, dzl: %f", dxl, dyl, dzl);

	for (int i = 0; i<4; i++)
	{
		ITBeamNode *n = new ITBeamNode(get_MyBeamNodes()->at(0)->get_X() + dxl*(i + 1),
			get_MyBeamNodes()->at(0)->get_Y() + dyl*(i + 1),
			get_MyBeamNodes()->at(0)->get_Z() + dzl*(i + 1));

		n->set_Mass(get_MyBeamNodes()->at(0)->get_Mass());

		// Copy across E, Ix and Iz from first element.
		n->set_E(get_MyBeamNodes()->at(0)->get_E());
		n->set_Ix(get_MyBeamNodes()->at(0)->get_Ix());
		n->set_Iz(get_MyBeamNodes()->at(0)->get_Iz());

		get_MyLeftGhostBeamNodes()->push_back(n);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside left createGhostBeamNodes. n->x: %f, n->y: %f, n->z: %f", n->get_X(), n->get_Y(), n->get_Z());

	}

	// Right.
	int N = get_MyBeamNodes()->size() - 1;
	float dxr = get_MyBeamNodes()->at(N)->get_X() - get_MyBeamNodes()->at(N - 1)->get_X();
	float dyr = get_MyBeamNodes()->at(N)->get_Y() - get_MyBeamNodes()->at(N - 1)->get_Y();
	float dzr = get_MyBeamNodes()->at(N)->get_Z() - get_MyBeamNodes()->at(N - 1)->get_Z();

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside createGhostBeamNodes. dxr: %f, dyr: %f, dzr: %f", dxr, dyr, dzr);

	for (int i = 0; i<4; i++)
	{
		ITBeamNode *n = new ITBeamNode(get_MyBeamNodes()->at(N)->get_X() + dxr*(i + 1),
			get_MyBeamNodes()->at(N)->get_Y() + dyr*(i + 1),
			get_MyBeamNodes()->at(N)->get_Z() + dzr*(i + 1));

		n->set_Mass(get_MyBeamNodes()->at(N)->get_Mass());

		// Copy across E, Ix and Iz from first element.
		n->set_E(get_MyBeamNodes()->at(N)->get_E());
		n->set_Ix(get_MyBeamNodes()->at(N)->get_Ix());
		n->set_Iz(get_MyBeamNodes()->at(N)->get_Iz());

		get_MyRightGhostBeamNodes()->push_back(n);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside right createGhostBeamNodes. n->x: %f, n->y: %f, n->z: %f", n->get_X(), n->get_Y(), n->get_Z());

	}


} // End of createGhostBeamNodes.



void ITBeam::propagateBeamNodes()
{
	int NO_OF_NODES = get_MyBeamNodes()->size();

	int colIndex = floor(get_MyITSurface()->get_MyControlPoints()->at(0).size() / 2);

	float xs = get_MyITSurface()->get_MyControlPoints()->at(0).at(colIndex)->get_X();
	float ys = get_MyITSurface()->get_MyControlPoints()->at(0).at(colIndex)->get_Y();
	float zs = get_MyITSurface()->get_MyControlPoints()->at(0).at(colIndex)->get_Z();

	float xe = get_MyITSurface()->get_MyControlPoints()->back().at(colIndex)->get_X();
	float ye = get_MyITSurface()->get_MyControlPoints()->back().at(colIndex)->get_Y();
	float ze = get_MyITSurface()->get_MyControlPoints()->back().at(colIndex)->get_Z();


	float dx = (xe - xs) / (NO_OF_NODES);
	float dy = (ye - ys) / (NO_OF_NODES);
	float dz = (ze - zs) / (NO_OF_NODES);

	for (int i = 0; i<NO_OF_NODES; i++)
	{
		ITBeamNode *n = get_MyBeamNodes()->at(i);
		n->set_X(xs + dx*i + dx / 2);
		n->set_Y(ys + dy*i + dy / 2);
		n->set_Z(zs + dz*i + dz / 2);
	}

} // End of propagateBeamNodes.




void ITBeam::propagateGhostBeamNodes()
{
	// Left.
	int NO_OF_NODES = get_MyLeftGhostBeamNodes()->size();

	float dxl = get_MyBeamNodes()->at(0)->get_X() - get_MyBeamNodes()->at(1)->get_X();
	float dyl = get_MyBeamNodes()->at(0)->get_Y() - get_MyBeamNodes()->at(1)->get_Y();
	float dzl = get_MyBeamNodes()->at(0)->get_Z() - get_MyBeamNodes()->at(1)->get_Z();

	for (int i = 0; i<NO_OF_NODES; i++)
	{
		ITBeamNode *n = get_MyLeftGhostBeamNodes()->at(i);

		n->set_X(get_MyBeamNodes()->at(0)->get_X() + dxl*(i + 1));
		n->set_Y(get_MyBeamNodes()->at(0)->get_Y() + dyl*(i + 1));
		n->set_Z(get_MyBeamNodes()->at(0)->get_Z() + dzl*(i + 1));
	}


	// Right.
	int N = get_MyBeamNodes()->size() - 1;
	NO_OF_NODES = get_MyRightGhostBeamNodes()->size();

	float dxr = get_MyBeamNodes()->at(N)->get_X() - get_MyBeamNodes()->at(N - 1)->get_X();
	float dyr = get_MyBeamNodes()->at(N)->get_Y() - get_MyBeamNodes()->at(N - 1)->get_Y();
	float dzr = get_MyBeamNodes()->at(N)->get_Z() - get_MyBeamNodes()->at(N - 1)->get_Z();


	for (int i = 0; i<NO_OF_NODES; i++)
	{
		ITBeamNode *n = get_MyRightGhostBeamNodes()->at(i);

		n->set_X(get_MyBeamNodes()->at(N)->get_X() + dxr*(i + 1));
		n->set_Y(get_MyBeamNodes()->at(N)->get_Y() + dyr*(i + 1));
		n->set_Z(get_MyBeamNodes()->at(N)->get_Z() + dzr*(i + 1));
	}

} // End of propagateGhostBeamNodes.



void ITBeam::computeMyLaxRichtmyerDeltaT()
{

	double currentDt = 100.0; // A large initial value.

							  // FIXME: The following loop excludes the end elements of the beam. See Matlab calcDt function.
	float L = get_MyBeamElements()->at(0)->length();

	for (int i = 1; i<get_MyBeamNodes()->size() - 1; i++)
	{
		ITBeamNode *nm1 = get_MyBeamNodes()->at(i - 1);
		ITBeamNode *n = get_MyBeamNodes()->at(i);
		ITBeamNode *np1 = get_MyBeamNodes()->at(i + 1);

		float m = get_MyBeamNodes()->at(i)->get_Mass();

		float Fm1 = nm1->get_E() * nm1->get_Ix();
		float F = n->get_E()   * n->get_Ix();
		float Fp1 = np1->get_E() * np1->get_Ix();

		double dt = L*L * sqrt(2 * m / ((11.0 / 12.0)*Fm1 + 5.0*F + (11.0 / 12.0)*Fp1));

		if (dt < currentDt)
		{
			currentDt = dt;
		}
	}

	// Save the value of currentDt in an instance variable.
	set_EB_dt(0.0005*currentDt);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside computeMyLaxRichtmyerDeltaT. EB_dt: %15.10f, L: %f", get_EB_dt(), L);

} // End of computeMyLaxRichtmyerDeltaT.

std::string ITBeam::get_Name() { return _Name; }
void ITBeam::set_Name(std::string s) { _Name = s; }

float ITBeam::get_U() { return _U; }
void ITBeam::set_U(float u) { _U = u; }

float ITBeam::get_V() { return _V; }
void ITBeam::set_V(float v) { _V = v; }

std::vector <ITBeamElement*> *ITBeam::get_MyBeamElements() { return _MyBeamElements; }
void ITBeam::set_MyBeamElements(std::vector <ITBeamElement*> *bes) { _MyBeamElements = bes; }

std::vector <ITBeamNode*> *ITBeam::get_MyBeamNodes() { return _MyBeamNodes; }
void ITBeam::set_MyBeamNodes(std::vector <ITBeamNode*> *bns) { _MyBeamNodes = bns; }

ITSurface *ITBeam::get_MyITSurface() { return _MyITSurface; }
void ITBeam::set_MyITSurface(ITSurface *s) { _MyITSurface = s; }

double ITBeam::get_EB_dt() { return _EB_dt; }
void ITBeam::set_EB_dt(double d) { _EB_dt = d; }

std::vector <ITBeamNode*> *ITBeam::get_MyLeftGhostBeamNodes() { return _MyLeftGhostBeamNodes; }
void ITBeam::set_MyLeftGhostBeamNodes(std::vector <ITBeamNode*> *bes) { _MyLeftGhostBeamNodes = bes; }

std::vector <ITBeamNode*> *ITBeam::get_MyRightGhostBeamNodes() { return _MyRightGhostBeamNodes; }
void ITBeam::set_MyRightGhostBeamNodes(std::vector <ITBeamNode*> *bes) { _MyRightGhostBeamNodes = bes; }

std::vector <ITBeamElement*> *ITBeam::get_MyLeftGhostBeamElements() { return _MyLeftGhostBeamElements; }
void ITBeam::set_MyLeftGhostBeamElements(std::vector <ITBeamElement*> *bes) { _MyLeftGhostBeamElements = bes; }

std::vector <ITBeamElement*> *ITBeam::get_MyRightGhostBeamElements() { return _MyRightGhostBeamElements; }
void ITBeam::set_MyRightGhostBeamElements(std::vector <ITBeamElement*> *bes) { _MyRightGhostBeamElements = bes; }

std::vector < std::vector <ITBeamNode*> > *ITBeam::get_MyNodeHistory() { return _MyNodeHistory; }
void ITBeam::set_MyNodeHistory(std::vector < std::vector <ITBeamNode*> > *h) { _MyNodeHistory = h; }
