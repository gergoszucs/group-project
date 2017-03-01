// Dom's includes.
#include "global.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITControlPoint.h"

ITSurface::ITSurface(void)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside constructor. ITSurface being created.");

	_MyControlPoints = new std::vector < std::vector <ITControlPoint*> >;
	_MyFocusControlPoints = new std::vector <ITControlPoint*>; // This contains pointers to control points. No new control points are created.

	_MyInterpolatedPoints = new std::vector < std::vector <ITPoint*> >;
	_MyInterpolatedNormals = new std::vector < std::vector <ITPoint*> >;

	_MyGaussianCurvature = new std::vector < std::vector <float> >;


	// Set instance variables.
	set_FuselageRadius(2.5);

}


ITSurface::~ITSurface(void)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside destructor. ITSurface being destroyed.");

	// Delete the contol points.
	int noOfRows = get_MyControlPoints()->size();
	for (int i=0; i<noOfRows; i++)
	{
		int noOfCols = get_MyControlPoints()->at(i).size();
		for (int j=0; j<noOfCols; j++)
		{
			delete get_MyControlPoints()->at(i).at(j);
		}
		get_MyControlPoints()->at(i).clear();
	}

	get_MyControlPoints()->clear();
	delete _MyControlPoints;



	// Delete focus vector.
	delete _MyFocusControlPoints;



	// Delete the interpolated points.
	noOfRows = get_MyInterpolatedPoints()->size();
	for (int i=0; i<noOfRows; i++)
	{
		int noOfCols = get_MyInterpolatedPoints()->at(i).size();
		for (int j=0; j<noOfCols; j++)
		{
			delete get_MyInterpolatedPoints()->at(i).at(j);
		}
		get_MyInterpolatedPoints()->at(i).clear();
	}

	get_MyInterpolatedPoints()->clear();
	delete _MyInterpolatedPoints;

	// Delete the interpolated normals.
	noOfRows = get_MyInterpolatedNormals()->size();
	for (int i=0; i<noOfRows; i++)
	{
		int noOfCols = get_MyInterpolatedNormals()->at(i).size();
		for (int j=0; j<noOfCols; j++)
		{
			delete get_MyInterpolatedNormals()->at(i).at(j);
		}
		get_MyInterpolatedNormals()->at(i).clear();
	}

	get_MyInterpolatedNormals()->clear();
	delete _MyInterpolatedNormals;


	// Delete Gaussian curvature.
	get_MyGaussianCurvature()->clear();
	delete _MyGaussianCurvature;

}



// Utilities.
void ITSurface::manageComputationOfInterpolatedPoints()
{
	computeMU();
	computeMV();
	computeURange();
	computeVRange();
	computeUPowerMatrix();
	computeVPowerMatrix();

	computeInterpolatedPoints();

} // End of manageComputationOfInterpolatedPoints.

void ITSurface::computeMU()
{

	// Find the size of the net of control points.
	int r = get_MyControlPoints()->size(); // Checked
	int n = r-1; // Checked	

	Eigen::MatrixXd MU = Eigen::MatrixXd::Constant(r,r,0.0); // Checked.

	// Assemble the entries of the MU matrix.
	for (int i=1; i<=r; i++) // 0 to r-1. Rows of MU.
	{
		for (int j=1; j<=r; j++) // 0 to r-1. Columns of MU.
		{
			int bin = factorial(n)/(factorial(j-1)*factorial(n-j+1)); // The binomial coefficient n C. Checked 
			int mult;
			if ((r-j-i+1) < 0)
			{
				mult = 0;
			}
			else
			{
				mult = factorial(r-j)/(factorial(i-1)*factorial(r-j-i+1));
			}

			int sig = pow(-1, i+j+r+1 );
			MU(i-1,j-1) = sig*bin*mult;

		}
	}

	set_MU(MU);

} // End of computeMU.


void ITSurface::computeMV()
{
	//	printf("Inside computeMV\n");

	// Find the size of the net of control points.
	int c = get_MyControlPoints()->at(0).size(); // Checked
	int m = c-1; // Checked

	// Initialize the MV matrix.
	Eigen::MatrixXd MV = Eigen::MatrixXd::Constant(c,c,0.0); // Checked.

	// Assemble the entries of the MU matrix.
	for (int i=1; i<=c; i++) // 0 to r-1. Rows of MU.
	{
		for (int j=1; j<=c; j++) // 0 to r-1. Columns of MU.
		{
			int bin = factorial(m)/(factorial(j-1)*factorial(m-j+1)); // The binomial coefficient n C. Checked 
			int mult;
			if ((c-j-i+1) < 0)
			{
				mult = 0;
			}
			else
			{
				mult = factorial(c-j)/(factorial(i-1)*factorial(c-j-i+1));
			}

			int sig = pow(-1, i+j+c+1 );
			MV(i-1,j-1) = sig*bin*mult;


		}
	}

	set_MV(MV);

} // End of computeMV.

int ITSurface::factorial(int n)
{
	int retval = 1;
	for (int i = n; i > 1; --i)
	{
		retval *= i;
	}
	return retval;
} // End of factorial.

void ITSurface::computeURange()
{

	// This method constructs a vector of values of u at which the interpolated points will be computed.

	std::vector <float> *ur = new std::vector<float>;

	float du = 	1.0f/(get_NoOfInterpolatedPointsU()-1);

	for (int i=0; i<get_NoOfInterpolatedPointsU(); i++)
	{
		ur->push_back(i*du);
	}
	set_URange(ur);

} // End of computeURange.

void ITSurface::computeVRange()
{

	// This method constructs a vector of values of v at which the interpolated points will be computed.

	std::vector <float> *vr = new std::vector<float>;

	float dv = 	1.0f/(get_NoOfInterpolatedPointsV()-1);

	for (int i=0; i<get_NoOfInterpolatedPointsV(); i++)
	{
		vr->push_back(i*dv);
	}
	set_VRange(vr);

} // End of computeVRange.



void ITSurface::computeUPowerMatrix()
{

	int r = get_MyControlPoints()->size(); // Checked
	int n = r-1; // Checked

	Eigen::MatrixXd upm = Eigen::MatrixXd::Constant(get_NoOfInterpolatedPointsU(), r, 1.0);

	for (int i=0; i<r; i++) // Columns
	{
		for (int j=0; j<get_NoOfInterpolatedPointsU(); j++) // Rows
		{
			upm(j, n-i) = pow(get_URange()->at(j), i);
		}
	}
	set_UPowerMatrix(upm);

} // End of computeUPowerMatrix.


void ITSurface::computeVPowerMatrix()
{

	int c = get_MyControlPoints()->at(0).size(); // Checked
	int m = c-1; // Checked

	Eigen::MatrixXd vpm = Eigen::MatrixXd::Constant(get_NoOfInterpolatedPointsV(), c, 1.0);

	for (int i=0; i<c; i++) // Columns
	{
		for (int j=0; j<get_NoOfInterpolatedPointsV(); j++) // Rows
		{
			vpm(j, m-i) = pow(get_VRange()->at(j), i);
		}
	}
	set_VPowerMatrix(vpm);

} // End of computeVPowerMatrix.




void ITSurface::computeInterpolatedPoints()
{

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside computeInterpolatedPoints");

	// First find my index in the project get_MySurfaces array;
	int k;
	for (int n=0; n<project->get_MySurfaces()->size(); n++)
	{
		if (project->get_MySurfaces()->at(n) == this) // Compare pointers.
		{
			// Match has been found.
			k = n;
			break;
		}
	}

	// Make sure the interpolated points and normals are empty before we start.
	if (!_MyInterpolatedPoints->empty())
	{
		for (int i=0; i<get_MyInterpolatedPoints()->size(); i++)
		{
			for (int j=0; j<get_MyInterpolatedPoints()->at(0).size(); j++)
			{
				delete _MyInterpolatedPoints->at(i).at(j);
				delete _MyInterpolatedNormals->at(i).at(j);
			}
			_MyInterpolatedPoints->at(i).clear();
			_MyInterpolatedNormals->at(i).clear();
		}

		_MyInterpolatedPoints->clear();
		_MyInterpolatedNormals->clear();
	}

	// Construct matrix of control point coordinate values.
	int rows = get_MyControlPoints()->size();
	int cols = get_MyControlPoints()->at(0).size();

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "rows: %i, cols: %i", rows, cols);

	// Allocate the memory for each of the matrices.
	Eigen::MatrixXd PX = Eigen::MatrixXd::Constant(rows, cols, 0.0);
	Eigen::MatrixXd PY = Eigen::MatrixXd::Constant(rows, cols, 0.0);
	Eigen::MatrixXd PZ = Eigen::MatrixXd::Constant(rows, cols, 0.0);

	// Fill the matrix elements with values.
	for (int i=0; i<rows; i++)
	{
		for (int j=0; j<cols; j++)
		{
			PX(i,j) = get_MyControlPoints()->at(i).at(j)->get_X();
			PY(i,j) = get_MyControlPoints()->at(i).at(j)->get_Y();
			PZ(i,j) = get_MyControlPoints()->at(i).at(j)->get_Z();
		}
	}

	// Compute the interpolated coordinates.
	Eigen::MatrixXd xb = get_UPowerMatrix() * get_MU() * PX * get_MV() * get_VPowerMatrix().transpose();
	Eigen::MatrixXd yb = get_UPowerMatrix() * get_MU() * PY * get_MV() * get_VPowerMatrix().transpose();
	Eigen::MatrixXd zb = get_UPowerMatrix() * get_MU() * PZ * get_MV() * get_VPowerMatrix().transpose();

	// Instanciate the vector of vectors of interpolated ITPoints.
	for (int i=0; i<get_NoOfInterpolatedPointsU(); i++)
	{

		std::vector <ITPoint *> v_dummy_points;
		std::vector <ITPoint *> v_dummy_normals;

		for (int j=0; j<get_NoOfInterpolatedPointsV(); j++)
		{
			// Create the interpolated points that will be used for adjustments.
			ITPoint *p = new ITPoint(xb(i,j), yb(i,j), zb(i,j));

			p->set_I(i);
			p->set_J(j);
			p->set_K(k);

			float du = 	1.0f/(get_NoOfInterpolatedPointsU()-1);
			float dv = 	1.0f/(get_NoOfInterpolatedPointsV()-1);

			p->set_U((float)i*du);
			p->set_V((float)j*dv);

			v_dummy_points.push_back(p);			

			// Compute normal vector.
			ITPoint *n = new ITPoint(0.0f, 0.0f, 0.0f);
			if ( (i<get_NoOfInterpolatedPointsU()-1) && (j<get_NoOfInterpolatedPointsV()-1) )
			{
				float dxu = xb(i+1,j) - xb(i,j);
				float dyu = yb(i+1,j) - yb(i,j);
				float dzu = zb(i+1,j) - zb(i,j);

				float dxv = xb(i,j+1) - xb(i,j);
				float dyv = yb(i,j+1) - yb(i,j);
				float dzv = zb(i,j+1) - zb(i,j);

				// Do cross product.
				float nx = dyv*dzu - dyu*dzv;
				float ny = dzv*dxu - dxv*dzu;
				float nz = dxv*dyu - dyv*dxu;

				n->set_X(nx);
				n->set_Y(ny);
				n->set_Z(nz);
				n->normalize();
			}
			else
			{
				n->set_X(0.0);
				n->set_Y(0.0);
				n->set_Z(0.0);
			}

			v_dummy_normals.push_back(n);
		}

		// Fill the vectors of vectors.
		_MyInterpolatedPoints->push_back(v_dummy_points);
		_MyInterpolatedNormals->push_back(v_dummy_normals);
	}

} // End of computeInterpolatedPoints.








// Accessors.
int ITSurface::get_NoOfInterpolatedPointsU(){ return _NoOfInterpolatedPointsU; }
void ITSurface::set_NoOfInterpolatedPointsU(int u){ _NoOfInterpolatedPointsU = u; }

int ITSurface::get_NoOfInterpolatedPointsV(){ return _NoOfInterpolatedPointsV; }
void ITSurface::set_NoOfInterpolatedPointsV(int v){ _NoOfInterpolatedPointsV = v; }

std::vector < std::vector <ITControlPoint*> > *ITSurface::get_MyControlPoints(){ return _MyControlPoints; }
void ITSurface::set_MyControlPoints(std::vector < std::vector <ITControlPoint*> > *myControlPoints){ _MyControlPoints = myControlPoints; }

std::vector <ITControlPoint*> *ITSurface::get_MyFocusControlPoints(){ return _MyFocusControlPoints; }
void ITSurface::set_MyFocusControlPoints(std::vector <ITControlPoint*> *myFocusControlPoints){ _MyFocusControlPoints = myFocusControlPoints; }

std::vector < std::vector <ITPoint*> > *ITSurface::get_MyInterpolatedPoints(){ return _MyInterpolatedPoints; }
void ITSurface::set_MyInterpolatedPoints(std::vector < std::vector <ITPoint*> > *myInterpolatedPoints){ _MyInterpolatedPoints = myInterpolatedPoints; }

std::vector < std::vector <ITPoint*> > *ITSurface::get_MyInterpolatedNormals(){ return _MyInterpolatedNormals; }
void ITSurface::set_MyInterpolatedNormals(std::vector < std::vector <ITPoint*> > *myInterpolatedNormals){ _MyInterpolatedNormals = myInterpolatedNormals; }

Eigen::MatrixXd ITSurface::get_MU(){ return _MU; }
void ITSurface::set_MU(Eigen::MatrixXd mu){ _MU = mu; }

Eigen::MatrixXd ITSurface::get_MV(){ return _MV; }
void ITSurface::set_MV(Eigen::MatrixXd mv){ _MV = mv; }

std::vector <float> *ITSurface::get_URange(){ return _URange; }
void ITSurface::set_URange(std::vector <float> *ur){ _URange = ur; }

std::vector <float> *ITSurface::get_VRange(){ return _VRange; }
void ITSurface::set_VRange(std::vector <float> *vr){ _VRange = vr; }

Eigen::MatrixXd ITSurface::get_UPowerMatrix(){ return _UPowerMatrix; }
void ITSurface::set_UPowerMatrix(Eigen::MatrixXd upm){ _UPowerMatrix = upm; }

Eigen::MatrixXd ITSurface::get_VPowerMatrix(){ return _VPowerMatrix; }
void ITSurface::set_VPowerMatrix(Eigen::MatrixXd vpm){ _VPowerMatrix = vpm; }

std::vector < std::vector <float> > *ITSurface::get_MyGaussianCurvature(){ return _MyGaussianCurvature; }
void ITSurface::set_MyGaussianCurvature(std::vector < std::vector <float> > *myGaussianCurvature){ _MyGaussianCurvature = myGaussianCurvature; }


bool ITSurface::get_IsMorph(){ return _IsMorph; }
void ITSurface::set_IsMorph(bool b){ _IsMorph = b; }

int ITSurface::get_MorphStartFrame(){ return _MorphStartFrame; }
void ITSurface::set_MorphStartFrame(int f){ _MorphStartFrame = f; }

int ITSurface::get_MorphEndFrame(){ return _MorphEndFrame; }
void ITSurface::set_MorphEndFrame(int f){ _MorphEndFrame = f; }

std::string ITSurface::get_MorphType(){ return _MorphType; }
void ITSurface::set_MorphType(std::string s){ _MorphType = s; }

bool ITSurface::get_IsWake(){ return _IsWake; }
void ITSurface::set_IsWake(bool b){ _IsWake = b; }

int ITSurface::get_ParentSurfaceIndex(){ return _ParentSurfaceIndex; }
void ITSurface::set_ParentSurfaceIndex(int f) { _ParentSurfaceIndex = f; }

bool ITSurface::get_IsPistolesiPanelOffset(){ return _IsPistolesiPanelOffset; }
void ITSurface::set_IsPistolesiPanelOffset(bool b){ _IsPistolesiPanelOffset = b; }

float ITSurface::get_PistolesiPanelOffsetFraction(){ return _PistolesiPanelOffsetFraction; }
void ITSurface::set_PistolesiPanelOffsetFraction(float p){ _PistolesiPanelOffsetFraction = p; }

float ITSurface::get_FuselageRadius(){ return _FuselageRadius; }
void ITSurface::set_FuselageRadius(float f){ _FuselageRadius = f; }
