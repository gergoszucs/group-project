#include "global.h"
#include "ITPanel.h"
#include "ITPoint.h"
#include "ITControlPoint.h"
#include "ITSurface.h"
#include "ITVortex.h"

ITPanel::ITPanel(ITPoint* bottomLeft, ITPoint* bottomRight, ITPoint* topRight, ITPoint* topLeft, ITSurface* s, int typeOfPanel)
{
	// Set my surface
	set_MyITSurface(s);

	// Compute panel geometry.
	// Instanciate the panel corner points with pointers to supplied objects.
	// At this point, botL, botR, topR and topL are coincident with the interpolated points.
	ITPoint *botL = new ITPoint(bottomLeft->get_X(), bottomLeft->get_Y(), bottomLeft->get_Z());
	ITPoint *botR = new ITPoint(bottomRight->get_X(), bottomRight->get_Y(), bottomRight->get_Z());
	ITPoint *topR = new ITPoint(topRight->get_X(), topRight->get_Y(), topRight->get_Z());
	ITPoint *topL = new ITPoint(topLeft->get_X(), topLeft->get_Y(), topLeft->get_Z());

	if ((get_MyITSurface()->get_IsPistolesiPanelOffset()) && (typeOfPanel == 0))
	{
		// Do Pistolesi computations for a bound panel.
		float xBL, yBL, zBL;
		float xBR, yBR, zBR;
		float xTR, yTR, zTR;
		float xTL, yTL, zTL;

		// Start with bottom left.
		xBL = botL->get_X() + s->get_PistolesiPanelOffsetFraction()*(botL->get_X() - botR->get_X());
		yBL = botL->get_Y() + s->get_PistolesiPanelOffsetFraction()*(botL->get_Y() - botR->get_Y());
		zBL = botL->get_Z() + s->get_PistolesiPanelOffsetFraction()*(botL->get_Z() - botR->get_Z());

		// Bottom right.
		xBR = botR->get_X() + s->get_PistolesiPanelOffsetFraction()*(botL->get_X() - botR->get_X());
		yBR = botR->get_Y() + s->get_PistolesiPanelOffsetFraction()*(botL->get_Y() - botR->get_Y());
		zBR = botR->get_Z() + s->get_PistolesiPanelOffsetFraction()*(botL->get_Z() - botR->get_Z());

		// Top right.
		xTR = topR->get_X() + s->get_PistolesiPanelOffsetFraction()*(topL->get_X() - topR->get_X());
		yTR = topR->get_Y() + s->get_PistolesiPanelOffsetFraction()*(topL->get_Y() - topR->get_Y());
		zTR = topR->get_Z() + s->get_PistolesiPanelOffsetFraction()*(topL->get_Z() - topR->get_Z());

		// Top left.
		xTL = topL->get_X() + s->get_PistolesiPanelOffsetFraction()*(topL->get_X() - topR->get_X());
		yTL = topL->get_Y() + s->get_PistolesiPanelOffsetFraction()*(topL->get_Y() - topR->get_Y());
		zTL = topL->get_Z() + s->get_PistolesiPanelOffsetFraction()*(topL->get_Z() - topR->get_Z());

		// Now re-assign coordinates of the botL etc points.
		botL->set_X(xBL);
		botL->set_Y(yBL);
		botL->set_Z(zBL);
		set_BottomLeftPoint(botL);

		botR->set_X(xBR);
		botR->set_Y(yBR);
		botR->set_Z(zBR);
		set_BottomRightPoint(botR);

		topR->set_X(xTR);
		topR->set_Y(yTR);
		topR->set_Z(zTR);
		set_TopRightPoint(topR);

		topL->set_X(xTL);
		topL->set_Y(yTL);
		topL->set_Z(zTL);
		set_TopLeftPoint(topL);
	}
	else
	{
		// This is either a wake panel or IsPistolesiPanelOffset is false.
		set_BottomLeftPoint(botL);
		set_BottomRightPoint(botR);
		set_TopRightPoint(topR);
		set_TopLeftPoint(topL);
	}

	// Instanciate the normal.
	set_Normal(new ITPoint(0.0, 0.0, 1.0));

	// Instanciate the mid point.
	set_MidPoint(new ITPoint(0.0, 0.0, 1.0));

	// Instanciate the mid point.
	set_ColocationPoint(new ITPoint(0.0, 0.0, 1.0));

	// Instanciate the earth velocity of the mid-point.
	set_MyMidPointEarthVelocity(new ITPoint(0.0, 0.0, 0.0));

	// Initialize the pressure.
	set_DeltaPressure(0.0);

	// Compute derived geometry.
	computeDiagonals();
	computeMidPoint();
	computeColocationPoint();
	computeNormal();
	computeArea();

	// Instanciate a vector to contain the four bound vortices.
	set_MyVortices(new std::vector <ITVortex*>);

	// Instanciate my vortices.
	instanciateMyVortices();

	// Initialize the value of the vorticity.
	set_MyVorticity(0.0);

	// Take account of the forces on this panel.
	set_IgnoreForces(false);

	// Set default as NOT belonging to a control surface.
	set_IsControlSurface(false);
}

ITPanel::~ITPanel(void)
{
	// The panel vertex instance variables are pointers to ITPoint objects, so they should be deleted here. 
	delete _bottomLeftPoint;
	delete _bottomRightPoint;
	delete _topLeftPoint;
	delete _topRightPoint;

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside destructor. ITPanel being destroyed.");

	delete _Normal;
	delete _midPoint;
	delete _colocationPoint;
	delete _MyMidPointEarthVelocity;

	// Delete my vortices.
	int noOfVortices = get_MyVortices()->size();
	for (int i = 0; i < noOfVortices; i++)
	{
		delete get_MyVortices()->at(i);
	}

	get_MyVortices()->clear();
	delete _MyVortices;
}

// Worker methods.
void ITPanel::computeDiagonals()
{
	// Compute Diagonals 
	// Leading diagonal is top left to bottom right "\".	
	ITPoint *leadingDiagonal = new ITPoint(
		get_BottomRightPoint()->get_X() - get_TopLeftPoint()->get_X(),
		get_BottomRightPoint()->get_Y() - get_TopLeftPoint()->get_Y(),
		get_BottomRightPoint()->get_Z() - get_TopLeftPoint()->get_Z());

	set_LeadingDiagonal(leadingDiagonal);

	// Trailing diagonal is top right to bottom left "/".
	ITPoint *trailingDiagonal = new ITPoint(
		get_BottomLeftPoint()->get_X() - get_TopRightPoint()->get_X(),
		get_BottomLeftPoint()->get_Y() - get_TopRightPoint()->get_Y(),
		get_BottomLeftPoint()->get_Z() - get_TopRightPoint()->get_Z());

	set_TrailingDiagonal(trailingDiagonal);
}

void ITPanel::computeMidPoint()
{
	// Compute panel middle point.
	float leadDiagMidPtX = get_TopLeftPoint()->get_X() + get_LeadingDiagonal()->get_X() / 2.0f;
	float leadDiagMidPtY = get_TopLeftPoint()->get_Y() + get_LeadingDiagonal()->get_Y() / 2.0f;
	float leadDiagMidPtZ = get_TopLeftPoint()->get_Z() + get_LeadingDiagonal()->get_Z() / 2.0f;

	float trailDiagMidPtX = get_TopRightPoint()->get_X() + get_TrailingDiagonal()->get_X() / 2.0f;
	float trailDiagMidPtY = get_TopRightPoint()->get_Y() + get_TrailingDiagonal()->get_Y() / 2.0f;
	float trailDiagMidPtZ = get_TopRightPoint()->get_Z() + get_TrailingDiagonal()->get_Z() / 2.0f;

	get_MidPoint()->set_X((leadDiagMidPtX + trailDiagMidPtX) / 2.0f);
	get_MidPoint()->set_Y((leadDiagMidPtY + trailDiagMidPtY) / 2.0f);
	get_MidPoint()->set_Z((leadDiagMidPtZ + trailDiagMidPtZ) / 2.0f);
}

void ITPanel::computeColocationPoint()
{
	get_ColocationPoint()->set_X(get_MidPoint()->get_X());
	get_ColocationPoint()->set_Y(get_MidPoint()->get_Y());
	get_ColocationPoint()->set_Z(get_MidPoint()->get_Z());
}

void ITPanel::computeNormal()
{
	// Compute the unit normal using cross product of the diagonals.
	// This way the normals are directed to Leeward.
	float dxu = get_LeadingDiagonal()->get_X();
	float dyu = get_LeadingDiagonal()->get_Y();
	float dzu = get_LeadingDiagonal()->get_Z();

	float dxv = get_TrailingDiagonal()->get_X();
	float dyv = get_TrailingDiagonal()->get_Y();
	float dzv = get_TrailingDiagonal()->get_Z();

	float nx = dyu*dzv - dyv*dzu;
	float ny = dzu*dxv - dxu*dzv;
	float nz = dxu*dyv - dyu*dxv;

	get_Normal()->set_X(nx);
	get_Normal()->set_Y(ny);
	get_Normal()->set_Z(nz);

	get_Normal()->normalize();
}

void ITPanel::computeArea()
{
	// Compute the area of the panel as the cross product of the left side and the bottom side.
	float xu = get_TopLeftPoint()->get_X() - get_BottomLeftPoint()->get_X();
	float yu = get_TopLeftPoint()->get_Y() - get_BottomLeftPoint()->get_Y();
	float zu = get_TopLeftPoint()->get_Z() - get_BottomLeftPoint()->get_Z();

	float xv = get_BottomRightPoint()->get_X() - get_BottomLeftPoint()->get_X();
	float yv = get_BottomRightPoint()->get_Y() - get_BottomLeftPoint()->get_Y();
	float zv = get_BottomRightPoint()->get_Z() - get_BottomLeftPoint()->get_Z();

	float ax = yu*zv - yv*zu;
	float ay = zu*xv - xu*zv;
	float az = xu*yv - yu*xv;

	set_Area(sqrt(ax*ax + ay*ay + az*az)); // Always positive.
}

void ITPanel::instanciateMyVortices()
{
	// Instanciate bound vortices.
	ITVortex *v1 = new ITVortex(get_BottomLeftPoint(), get_BottomRightPoint());
	v1->set_MyITPanel(this);

	ITVortex *v2 = new ITVortex(get_BottomRightPoint(), get_TopRightPoint());
	v2->set_MyITPanel(this);

	ITVortex *v3 = new ITVortex(get_TopRightPoint(), get_TopLeftPoint());
	v3->set_MyITPanel(this);

	ITVortex *v4 = new ITVortex(get_TopLeftPoint(), get_BottomLeftPoint());
	v4->set_MyITPanel(this);

	get_MyVortices()->push_back(v1);
	get_MyVortices()->push_back(v2);
	get_MyVortices()->push_back(v3);
	get_MyVortices()->push_back(v4);
}

void ITPanel::calculateMyTrajectoryVelocity(int k)
{
	// Input parameter k is the index of the parent surface of this ITPanel object.
	// Find the velocity of the mid point of the panel at the current FrameNumber.
	// Start by finding the position of the mid point of the panel at the previous frame and at the current frame and taking the difference.
	// Create a convenient pointer to the coordinates of the first control point.
	// This is the "datum" point of the parent surface.
	ITPoint * cp = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(0).at(0);

	// Instanciate a temporary local ITPoint object to contain the location of
	// the mid point of this panel at the previous frame.
	ITPoint *previousBaseMidPoint = new ITPoint(0.0f, 0.0f, 0.0f);

	// Get the coordinates of the current panel mid point base position and store them in the
	// previousMidPoint temporary object.
	previousBaseMidPoint->set_X(project->get_MyBaseSurfaces()->at(k)->get_MyPanels()->at(get_I()).at(get_J())->get_MidPoint()->get_X());
	previousBaseMidPoint->set_Y(project->get_MyBaseSurfaces()->at(k)->get_MyPanels()->at(get_I()).at(get_J())->get_MidPoint()->get_Y());
	previousBaseMidPoint->set_Z(project->get_MyBaseSurfaces()->at(k)->get_MyPanels()->at(get_I()).at(get_J())->get_MidPoint()->get_Z());

	// Calculate the position of this panel temporary mid point propagated to the previous frame.
	previousBaseMidPoint->propagateMeWithMorphForTrajectorySpeed(cp,
		get_MyITSurface()->get_MyPreviousRotationPoint(),
		get_MyITSurface()->get_MyPreviousTranslationPoint(),
		get_MyITSurface()->get_IsMorph(),
		get_MyITSurface()->get_MorphStartFrame(),
		get_MyITSurface()->get_MorphEndFrame(),
		get_MyITSurface()->get_MorphType(),
		k,
		FrameNumber - 1);

	// Instanciate a temporary local ITPoint object to contain the location of
	// the mid point of this panel at the current frame.
	ITPoint *nowBaseMidPoint = new ITPoint(0.0f, 0.0f, 0.0f);

	// Assign the coordinates of the current panel mid point base position to the nowPoint temporary object.
	nowBaseMidPoint->set_X(project->get_MyBaseSurfaces()->at(k)->get_MyPanels()->at(get_I()).at(get_J())->get_MidPoint()->get_X());
	nowBaseMidPoint->set_Y(project->get_MyBaseSurfaces()->at(k)->get_MyPanels()->at(get_I()).at(get_J())->get_MidPoint()->get_Y());
	nowBaseMidPoint->set_Z(project->get_MyBaseSurfaces()->at(k)->get_MyPanels()->at(get_I()).at(get_J())->get_MidPoint()->get_Z());

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "k: %i, now base mid-point X: %f", k,
		project->get_MyBaseSurfaces()->at(k)->get_MyPanels()->at(get_I()).at(get_J())->get_MidPoint()->get_X());

	// Calculate the position of this panel temporary mid point propagated to this frame.
	nowBaseMidPoint->propagateMeWithMorphForTrajectorySpeed(cp,
		get_MyITSurface()->get_MyCurrentRotationPoint(),
		get_MyITSurface()->get_MyCurrentTranslationPoint(),
		get_MyITSurface()->get_IsMorph(),
		get_MyITSurface()->get_MorphStartFrame(),
		get_MyITSurface()->get_MorphEndFrame(),
		get_MyITSurface()->get_MorphType(),
		k,
		FrameNumber);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12,
		"k: %i, now X: %f, previous X: %f, get_I: %i, get_J: %i, CurrentTranslationPoint()->get_X(): %f, PreviousTranslationPoint()->get_X(): %f",
		k, nowBaseMidPoint->get_X(), previousBaseMidPoint->get_X(), get_I(), get_J(), get_MyITSurface()->get_MyCurrentTranslationPoint()->get_X(),
		get_MyITSurface()->get_MyPreviousTranslationPoint()->get_X());

	// Subtract previous position from current position.
	float deltaX = nowBaseMidPoint->get_X() - previousBaseMidPoint->get_X();
	float deltaY = nowBaseMidPoint->get_Y() - previousBaseMidPoint->get_Y();
	float deltaZ = nowBaseMidPoint->get_Z() - previousBaseMidPoint->get_Z();

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "k: %i, deltaX: %f, deltaY: %f, deltaZ: %f", k, deltaX, deltaY, deltaZ);

	// Multiply by frames per second to get velocity in metres per seconds.
	get_MyMidPointEarthVelocity()->set_X(deltaX*project->get_FramesPerSecond());
	get_MyMidPointEarthVelocity()->set_Y(deltaY*project->get_FramesPerSecond());
	get_MyMidPointEarthVelocity()->set_Z(deltaZ*project->get_FramesPerSecond());

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "k: %i, earth v: %f, %f, %f", k, get_MyMidPointEarthVelocity()->get_X(),
		get_MyMidPointEarthVelocity()->get_Y(), get_MyMidPointEarthVelocity()->get_Z());

	// FIXME: We need to add deflection velocities here. W->get_VZ().

	// Delete temporary objects.
	delete previousBaseMidPoint;
	delete nowBaseMidPoint;
}

ITSurface *ITPanel::get_MyITSurface() { return _MyITSurface; }
void ITPanel::set_MyITSurface(ITSurface *s) { _MyITSurface = s; }

ITPoint *ITPanel::get_BottomLeftPoint() { return _bottomLeftPoint; }
void ITPanel::set_BottomLeftPoint(ITPoint *p) { _bottomLeftPoint = p; }

ITPoint *ITPanel::get_BottomRightPoint() { return _bottomRightPoint; }
void ITPanel::set_BottomRightPoint(ITPoint *p) { _bottomRightPoint = p; }

ITPoint *ITPanel::get_TopLeftPoint() { return _topLeftPoint; }
void ITPanel::set_TopLeftPoint(ITPoint *p) { _topLeftPoint = p; }

ITPoint *ITPanel::get_TopRightPoint() { return _topRightPoint; }
void ITPanel::set_TopRightPoint(ITPoint *p) { _topRightPoint = p; }

std::vector <ITVortex*> *ITPanel::get_MyVortices() { return _MyVortices; }
void ITPanel::set_MyVortices(std::vector <ITVortex*> *v) { _MyVortices = v; }

ITPoint *ITPanel::get_LeadingDiagonal() { return _leadingDiagonal; }
void ITPanel::set_LeadingDiagonal(ITPoint *p) { _leadingDiagonal = p; }

ITPoint *ITPanel::get_TrailingDiagonal() { return _trailingDiagonal; }
void ITPanel::set_TrailingDiagonal(ITPoint *p) { _trailingDiagonal = p; }

ITPoint *ITPanel::get_MidPoint() { return _midPoint; }
void ITPanel::set_MidPoint(ITPoint *p) { _midPoint = p; }

ITPoint *ITPanel::get_ColocationPoint() { return _colocationPoint; }
void ITPanel::set_ColocationPoint(ITPoint *p) { _colocationPoint = p; }

ITPoint *ITPanel::get_MyMidPointEarthVelocity() { return _MyMidPointEarthVelocity; }
void ITPanel::set_MyMidPointEarthVelocity(ITPoint *p) { _MyMidPointEarthVelocity = p; }

ITPoint *ITPanel::get_Normal() { return _Normal; }
void ITPanel::set_Normal(ITPoint *p) { _Normal = p; }

float ITPanel::get_Area() { return _Area; }
void ITPanel::set_Area(float a) { _Area = a; }

float ITPanel::get_DeltaPressure() { return _DeltaPressure; }
void ITPanel::set_DeltaPressure(float p) { _DeltaPressure = p; }

float ITPanel::get_MyVorticity() { return _MyVorticity; }
void ITPanel::set_MyVorticity(float v) { _MyVorticity = v; }

int ITPanel::get_I() { return _i; }
void ITPanel::set_I(int i) { _i = i; }

int ITPanel::get_J() { return _j; }
void ITPanel::set_J(int j) { _j = j; }

int ITPanel::get_K() { return _k; }
void ITPanel::set_K(int k) { _k = k; }

bool ITPanel::get_IgnoreForces() { return _IgnoreForces; }
void ITPanel::set_IgnoreForces(bool k) { _IgnoreForces = k; }

bool ITPanel::get_IsControlSurface() { return _IsControlSurface; }
void ITPanel::set_IsControlSurface(bool isControlSurface) { _IsControlSurface = isControlSurface; }