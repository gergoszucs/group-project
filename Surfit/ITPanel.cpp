// Dominique's includes
#include "global.h"
#include "ITPanel.h"
#include "ITPoint.h"
#include "ITControlPoint.h"
#include "ITSurface.h"

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

	set_BottomLeftPoint(botL);
	set_BottomRightPoint(botR);
	set_TopRightPoint(topR);
	set_TopLeftPoint(topL);

	// Instanciate the normal.
	set_Normal( new ITPoint(0.0, 0.0, 1.0) );

	// Instanciate the mid point.
	set_MidPoint( new ITPoint(0.0, 0.0, 1.0) );

	// Instanciate the mid point.
	set_ColocationPoint( new ITPoint(0.0, 0.0, 1.0) );

	// Compute derived geometry.
	computeDiagonals();
	computeMidPoint();
	computeNormal();
	computeArea();

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
	float leadDiagMidPtX = get_TopLeftPoint()->get_X() + get_LeadingDiagonal()->get_X()/2.0f;
	float leadDiagMidPtY = get_TopLeftPoint()->get_Y() + get_LeadingDiagonal()->get_Y()/2.0f;
	float leadDiagMidPtZ = get_TopLeftPoint()->get_Z() + get_LeadingDiagonal()->get_Z()/2.0f;

	float trailDiagMidPtX = get_TopRightPoint()->get_X() + get_TrailingDiagonal()->get_X()/2.0f;
	float trailDiagMidPtY = get_TopRightPoint()->get_Y() + get_TrailingDiagonal()->get_Y()/2.0f;
	float trailDiagMidPtZ = get_TopRightPoint()->get_Z() + get_TrailingDiagonal()->get_Z()/2.0f;

	get_MidPoint()->set_X((leadDiagMidPtX + trailDiagMidPtX)/2.0f);
	get_MidPoint()->set_Y((leadDiagMidPtY + trailDiagMidPtY)/2.0f);
	get_MidPoint()->set_Z((leadDiagMidPtZ + trailDiagMidPtZ)/2.0f);

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

	set_Area( sqrt(ax*ax + ay*ay + az*az) ); // Always positive.

}










// Accessors.
ITSurface *ITPanel::get_MyITSurface(){ return _MyITSurface; }
void ITPanel::set_MyITSurface(ITSurface *s){ _MyITSurface = s; }

ITPoint *ITPanel::get_BottomLeftPoint(){ return _bottomLeftPoint; }
void ITPanel::set_BottomLeftPoint(ITPoint *p){ _bottomLeftPoint = p; }

ITPoint *ITPanel::get_BottomRightPoint(){ return _bottomRightPoint; }
void ITPanel::set_BottomRightPoint(ITPoint *p){ _bottomRightPoint = p; }

ITPoint *ITPanel::get_TopLeftPoint(){ return _topLeftPoint; }
void ITPanel::set_TopLeftPoint(ITPoint *p){ _topLeftPoint = p; }

ITPoint *ITPanel::get_TopRightPoint(){ return _topRightPoint; }
void ITPanel::set_TopRightPoint(ITPoint *p){ _topRightPoint = p; }

ITPoint *ITPanel::get_LeadingDiagonal(){ return _leadingDiagonal; }
void ITPanel::set_LeadingDiagonal(ITPoint *p){ _leadingDiagonal = p; }

ITPoint *ITPanel::get_TrailingDiagonal(){ return _trailingDiagonal; }
void ITPanel::set_TrailingDiagonal(ITPoint *p){ _trailingDiagonal = p; }

ITPoint *ITPanel::get_MidPoint(){ return _midPoint; }
void ITPanel::set_MidPoint(ITPoint *p){ _midPoint = p; }

ITPoint *ITPanel::get_ColocationPoint(){ return _colocationPoint; }
void ITPanel::set_ColocationPoint(ITPoint *p){ _colocationPoint = p; }

ITPoint *ITPanel::get_Normal(){ return _Normal; }
void ITPanel::set_Normal(ITPoint *p){ _Normal = p; }

float ITPanel::get_Area(){ return _Area; }
void ITPanel::set_Area(float a){ _Area = a; }

int ITPanel::get_I(){ return _i; }
void ITPanel::set_I(int i){ _i = i; }

int ITPanel::get_J(){ return _j; }
void ITPanel::set_J(int j){ _j = j; }

int ITPanel::get_K(){ return _k; }
void ITPanel::set_K(int k){ _k = k; }
