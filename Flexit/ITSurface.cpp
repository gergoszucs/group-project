#include "global.h"
#include "ITSurface.h"
#include "ITControlPoint.h"
#include "ITPanel.h"
#include "ITWakePanel.h"
#include "ITPointTrajectory.h"
#include "ITTrajectoryCurve.h"
#include "ITTrajectoryCurveSegment.h"
#include "ITPhysics.h" // For matrix multiplication in transformation.
#include "ITBeam.h"
#include "ITBeamNode.h"
#include "ITBeamElement.h"
#include "ITVortex.h"
#include "ITGust.h"
#include "CudaPhysics.cuh"

ITSurface::ITSurface(void)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside constructor. ITSurface being created.");

	_MyControlPoints = new std::vector < std::vector <ITControlPoint*> >;
	_MyTrajectoryCurves = new std::vector <ITTrajectoryCurve *>;
	_MyAncestorSurfaceIndices = new std::vector <int>;
	_MyInterpolatedPoints = new std::vector < std::vector <ITPoint*> >;
	_MyInterpolatedNormals = new std::vector < std::vector <ITPoint*> >;
	_MyPanels = new std::vector < std::vector <ITPanel*> >;
	_MyWakePanels = new std::vector < std::vector <ITWakePanel*> >;
	_MyControlSurfaces = new std::vector <ITControlSurface*>;

	_MyWakePanelHistory = new std::vector < std::vector < std::vector <ITWakePanel*> > >; // The history of wake panels used in VLM.
	_MyControlPointDeflectionHistory = new std::vector < std::vector < std::vector <ITPoint*> > >; // The history of control point deflections.
	_MyPressureHistory = new std::vector < std::vector < std::vector <float> > >; // The history of surface panels pressures used in VLM.
	_MyVorticityHistory = new std::vector < std::vector < std::vector <float> > >; // The history of surface panels pressures used in VLM.
	_MyCLHistory = new std::vector <float>;
	_MyCDHistory = new std::vector <float>;
	_MyForceHistory = new std::vector <ITPoint*>;

	_MyCentreOfRotationPoint = new ITPoint(0.0, 0.0, 0.0);

	// Initialize the translation and rotation trajectory points for the current and previous frames.
	ITPoint* ctp = new ITPoint(0.0, 0.0, 0.0);
	set_MyCurrentTranslationPoint(ctp);
	ITPoint* crp = new ITPoint(0.0, 0.0, 0.0);
	set_MyCurrentRotationPoint(crp);
	ITPoint* ptp = new ITPoint(0.0, 0.0, 0.0);
	set_MyPreviousTranslationPoint(ptp);
	ITPoint* prp = new ITPoint(0.0, 0.0, 0.0);
	set_MyPreviousRotationPoint(prp);

	ITBeam* b = new ITBeam(this);
	set_MyBeam(b);
}

ITSurface::~ITSurface(void)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside destructor. ITSurface being destroyed.");

	// Delete the contol points.
	int noOfRows = get_MyControlPoints()->size();
	for (int i = 0; i < noOfRows; i++)
	{
		int noOfCols = get_MyControlPoints()->at(i).size();
		for (int j = 0; j < noOfCols; j++)
		{
			delete get_MyControlPoints()->at(i).at(j);
		}
		get_MyControlPoints()->at(i).clear();
	}

	get_MyControlPoints()->clear();
	delete _MyControlPoints;

	// Delete the interpolated points.
	noOfRows = get_MyInterpolatedPoints()->size();
	for (int i = 0; i < noOfRows; i++)
	{
		int noOfCols = get_MyInterpolatedPoints()->at(i).size();
		for (int j = 0; j < noOfCols; j++)
		{
			delete get_MyInterpolatedPoints()->at(i).at(j);
		}
		get_MyInterpolatedPoints()->at(i).clear();
	}

	get_MyInterpolatedPoints()->clear();
	delete _MyInterpolatedPoints;

	// Delete the interpolated normals.
	noOfRows = get_MyInterpolatedNormals()->size();
	for (int i = 0; i < noOfRows; i++)
	{
		int noOfCols = get_MyInterpolatedNormals()->at(i).size();
		for (int j = 0; j < noOfCols; j++)
		{
			delete get_MyInterpolatedNormals()->at(i).at(j);
		}
		get_MyInterpolatedNormals()->at(i).clear();
	}

	get_MyInterpolatedNormals()->clear();
	delete _MyInterpolatedNormals;

	// Delte trajectory curves.
	int noOfTrajCurves = get_MyTrajectoryCurves()->size();
	for (int i = 0; i < noOfTrajCurves; i++)
	{
		delete get_MyTrajectoryCurves()->at(i);
	}

	get_MyTrajectoryCurves()->clear();
	delete _MyTrajectoryCurves;

	// Delete the ITPanels.
	noOfRows = get_MyPanels()->size();
	for (int i = 0; i < noOfRows; i++)
	{
		int noOfCols = get_MyPanels()->at(i).size();
		for (int j = 0; j < noOfCols; j++)
		{
			delete get_MyPanels()->at(i).at(j);
		}
		get_MyPanels()->at(i).clear();
	}

	get_MyPanels()->clear();
	delete _MyPanels;

	// Delete the ITWakePanels.
	noOfRows = get_MyWakePanels()->size();
	for (int i = 0; i < noOfRows; i++)
	{
		int noOfCols = get_MyWakePanels()->at(i).size();
		for (int j = 0; j < noOfCols; j++)
		{
			delete get_MyWakePanels()->at(i).at(j);
		}
		get_MyWakePanels()->at(i).clear();
	}

	get_MyWakePanels()->clear();
	delete _MyWakePanels;

	// Delete _MyControlSurfaces.
	int noOfPts = get_MyControlSurfaces()->size();
	for (int i = 0; i < noOfPts; i++)
	{
		delete get_MyControlSurfaces()->at(i);
	}

	get_MyControlSurfaces()->clear();
	delete _MyControlSurfaces;

	// Delete the WakePanelHistory.
	int noOfGens = get_MyWakePanelHistory()->size();
	for (int g = 0; g < noOfGens; g++)
	{
		noOfRows = get_MyWakePanelHistory()->at(g).size();
		for (int i = 0; i < noOfRows; i++)
		{
			int noOfCols = get_MyWakePanelHistory()->at(g).at(i).size();
			for (int j = 0; j < noOfCols; j++)
			{
				delete get_MyWakePanelHistory()->at(g).at(i).at(j);
			}
			get_MyWakePanelHistory()->at(g).at(i).clear();
		}
		get_MyWakePanelHistory()->at(g).clear();
	}
	get_MyWakePanelHistory()->clear();
	delete _MyWakePanelHistory;

	// Delete _MyControlPointDeflectionHistory
	noOfGens = get_MyControlPointDeflectionHistory()->size();
	for (int g = 0; g < noOfGens; g++)
	{
		noOfRows = get_MyControlPointDeflectionHistory()->at(g).size();
		for (int i = 0; i < noOfRows; i++)
		{
			int noOfCols = get_MyControlPointDeflectionHistory()->at(g).at(i).size();
			for (int j = 0; j < noOfCols; j++)
			{
				delete get_MyControlPointDeflectionHistory()->at(g).at(i).at(j);
			}
			get_MyControlPointDeflectionHistory()->at(g).at(i).clear();
		}
		get_MyControlPointDeflectionHistory()->at(g).clear();
	}
	get_MyControlPointDeflectionHistory()->clear();
	delete _MyControlPointDeflectionHistory;

	// Delete the PressureHistory.
	noOfGens = get_MyPressureHistory()->size();
	for (int g = 0; g < noOfGens; g++)
	{
		noOfRows = get_MyPressureHistory()->at(g).size();
		for (int i = 0; i < noOfRows; i++)
		{
			get_MyPressureHistory()->at(g).at(i).clear();
		}
		get_MyPressureHistory()->at(g).clear();
	}
	get_MyPressureHistory()->clear();
	delete _MyPressureHistory;

	// Delete the VorticityHistory.
	noOfGens = get_MyVorticityHistory()->size();
	for (int g = 0; g < noOfGens; g++)
	{
		noOfRows = get_MyVorticityHistory()->at(g).size();
		for (int i = 0; i < noOfRows; i++)
		{
			get_MyVorticityHistory()->at(g).at(i).clear();
		}
		get_MyVorticityHistory()->at(g).clear();
	}
	get_MyVorticityHistory()->clear();
	delete _MyVorticityHistory;

	// Delete _MyCLHistory
	get_MyCLHistory()->clear();
	delete _MyCLHistory;

	// Delete _MyCDHistory
	get_MyCDHistory()->clear();
	delete _MyCDHistory;

	// Delete _MyForceHistory.
	noOfPts = get_MyForceHistory()->size();
	for (int i = 0; i < noOfPts; i++)
	{
		delete get_MyForceHistory()->at(i);
	}

	get_MyForceHistory()->clear();
	delete _MyForceHistory;

	// Delete the ancestor surface index vector.
	get_MyAncestorSurfaceIndices()->clear();
	delete _MyAncestorSurfaceIndices;

	// Delete the centre of rotation point.
	delete _MyCentreOfRotationPoint;

	delete _MyCurrentTranslationPoint;
	delete _MyCurrentRotationPoint;
	delete _MyPreviousTranslationPoint;
	delete _MyPreviousRotationPoint;

	// Delete the beam.
	delete _MyBeam;
}

void ITSurface::prepareForSimulation()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside prepareForSimulation");

	// Empty wake panels and delete ITWakePanel objects.
	// Delete the ITWakePanels.
	int noOfRows = get_MyWakePanels()->size();
	for (int i = 0; i < noOfRows; i++)
	{
		int noOfCols = get_MyWakePanels()->at(i).size();
		for (int j = 0; j < noOfCols; j++)
		{
			delete get_MyWakePanels()->at(i).at(j);
		}
		get_MyWakePanels()->at(i).clear();
	}

	get_MyWakePanels()->clear();

	// Empty Force History data.
	int noOfFrames = get_MyForceHistory()->size();
	for (int i = 0; i < noOfFrames; i++)
	{
		delete get_MyForceHistory()->at(i);
	}
	get_MyForceHistory()->clear();

	// Delete the WakePanelHistory.
	int noOfGens = get_MyWakePanelHistory()->size();
	for (int g = 0; g < noOfGens; g++)
	{
		noOfRows = get_MyWakePanelHistory()->at(g).size();
		for (int i = 0; i < noOfRows; i++)
		{
			int noOfCols = get_MyWakePanelHistory()->at(g).at(i).size();
			for (int j = 0; j < noOfCols; j++)
			{
				delete get_MyWakePanelHistory()->at(g).at(i).at(j);
			}
			get_MyWakePanelHistory()->at(g).at(i).clear();
		}
		get_MyWakePanelHistory()->at(g).clear();
	}
	get_MyWakePanelHistory()->clear();

	// Delete the ControlPointDeflectionHistory.
	noOfGens = get_MyControlPointDeflectionHistory()->size();
	for (int g = 0; g < noOfGens; g++)
	{
		noOfRows = get_MyControlPointDeflectionHistory()->at(g).size();
		for (int i = 0; i < noOfRows; i++)
		{
			int noOfCols = get_MyControlPointDeflectionHistory()->at(g).at(i).size();
			for (int j = 0; j < noOfCols; j++)
			{
				delete get_MyControlPointDeflectionHistory()->at(g).at(i).at(j);
			}
			get_MyControlPointDeflectionHistory()->at(g).at(i).clear();
		}
		get_MyControlPointDeflectionHistory()->at(g).clear();
	}
	get_MyControlPointDeflectionHistory()->clear();

	// Delete the PressureHistory.
	noOfGens = get_MyPressureHistory()->size();
	for (int g = 0; g < noOfGens; g++)
	{
		noOfRows = get_MyPressureHistory()->at(g).size();
		for (int i = 0; i < noOfRows; i++)
		{
			get_MyPressureHistory()->at(g).at(i).clear();
		}
		get_MyPressureHistory()->at(g).clear();
	}
	get_MyPressureHistory()->clear();

	// Delete the VorticityHistory.
	noOfGens = get_MyVorticityHistory()->size();
	for (int g = 0; g < noOfGens; g++)
	{
		noOfRows = get_MyVorticityHistory()->at(g).size();
		for (int i = 0; i < noOfRows; i++)
		{
			get_MyVorticityHistory()->at(g).at(i).clear();
		}
		get_MyVorticityHistory()->at(g).clear();
	}
	get_MyVorticityHistory()->clear();

	// Delete _MyCLHistory
	get_MyCLHistory()->clear();

	// Delete _MyCDHistory
	get_MyCDHistory()->clear();

	//// Compute EB parameters.
	//if (IsModeEulerBernoulli)
	//{
	//	get_MyBeam()->computeMyLaxRichtmyerDeltaT();
	//}

	// Reset beam deflections.
	//noOfRows = get_MyBeam()->get_MyBeamNodes()->size();
	//for (int i = 0; i<noOfRows; i++)
	//{
	//	get_MyBeam()->get_MyBeamNodes()->at(i)->get_W()->set_X(0.0);
	//	get_MyBeam()->get_MyBeamNodes()->at(i)->get_W()->set_Y(0.0);
	//	get_MyBeam()->get_MyBeamNodes()->at(i)->get_W()->set_Z(0.0);
	//}

	// Reset control point deflections.
	noOfRows = get_MyControlPoints()->size();
	for (int i = 0; i < noOfRows; i++)
	{
		int noOfCols = get_MyControlPoints()->at(i).size();
		for (int j = 0; j < noOfCols; j++)
		{
			get_MyControlPoints()->at(i).at(j)->get_W()->set_X(0.0);
			get_MyControlPoints()->at(i).at(j)->get_W()->set_Y(0.0);
			get_MyControlPoints()->at(i).at(j)->get_W()->set_Z(0.0);
		}
	}

	// Reset pressures and Vorticities.
	noOfRows = get_MyPanels()->size();
	for (int i = 0; i < noOfRows; i++)
	{
		int noOfCols = get_MyPanels()->at(i).size();
		for (int j = 0; j < noOfCols; j++)
		{
			get_MyPanels()->at(i).at(j)->set_MyVorticity(0.0);
			get_MyPanels()->at(i).at(j)->set_DeltaPressure(0.0);
		}
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "End of  prepareForSimulation");
}

// Utilities.
void ITSurface::manageComputationOfInterpolatedPoints()
{
	// Apply deformations temporarily.
	for (int i = 0; i < get_MyControlPoints()->size(); i++)
	{
		for (int j = 0; j < get_MyControlPoints()->at(i).size(); j++)
		{
			ITControlPoint *p = get_MyControlPoints()->at(i).at(j);

			p->set_X(p->get_X() + p->get_W()->get_X());
			p->set_Y(p->get_Y() + p->get_W()->get_Y());
			p->set_Z(p->get_Z() + p->get_W()->get_Z());

			// project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Applying deformation i: %i, j: %i, p->get_W()->get_Z(): %f", i, j,  p->get_W()->get_Z());
		}
	}

	// Compute the interpolated points.
	computeMU();
	computeMV();
	computeURange();
	computeVRange();
	computeUPowerMatrix();
	computeVPowerMatrix();

	computeInterpolatedPoints();

	// Remove deformations.
	for (int i = 0; i < get_MyControlPoints()->size(); i++)
	{
		for (int j = 0; j < get_MyControlPoints()->at(i).size(); j++)
		{
			ITControlPoint *p = get_MyControlPoints()->at(i).at(j);
			p->set_X(p->get_X() - p->get_W()->get_X());
			p->set_Y(p->get_Y() - p->get_W()->get_Y());
			p->set_Z(p->get_Z() - p->get_W()->get_Z());

			// project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Removing deformation i: %i, j: %i, p->get_W()->get_Z(): %f", i, j,  p->get_W()->get_Z());
		}
	}
}

void ITSurface::computeMU()
{
	// Find the size of the net of control points.
	int r = get_MyControlPoints()->size(); // Checked
	int n = r - 1; // Checked	

	Eigen::MatrixXd MU = Eigen::MatrixXd::Constant(r, r, 0.0); // Checked.
	
	// Assemble the entries of the MU matrix.
	for (int i = 1; i <= r; i++) // 0 to r-1. Rows of MU.
	{
		for (int j = 1; j <= r; j++) // 0 to r-1. Columns of MU.
		{
			int bin = factorial(n) / (factorial(j - 1)*factorial(n - j + 1)); // The binomial coefficient n C. Checked 
			int mult;
			if ((r - j - i + 1) < 0)
			{
				mult = 0;
			}
			else
			{
				mult = factorial(r - j) / (factorial(i - 1)*factorial(r - j - i + 1));
			}

			int sig = pow(-1, i + j + r + 1);
			MU(i - 1, j - 1) = sig*bin*mult;
		}
	}

	set_MU(MU);
}

void ITSurface::computeMV()
{
	//	printf("Inside computeMV\n");
	// Find the size of the net of control points.
	int c = get_MyControlPoints()->at(0).size(); // Checked
	int m = c - 1; // Checked

	// Initialize the MV matrix.
	Eigen::MatrixXd MV = Eigen::MatrixXd::Constant(c, c, 0.0); // Checked.

	// Assemble the entries of the MU matrix.
	for (int i = 1; i <= c; i++) // 0 to r-1. Rows of MU.
	{
		for (int j = 1; j <= c; j++) // 0 to r-1. Columns of MU.
		{
			int bin = factorial(m) / (factorial(j - 1)*factorial(m - j + 1)); // The binomial coefficient n C. Checked 
			int mult;
			if ((c - j - i + 1) < 0)
			{
				mult = 0;
			}
			else
			{
				mult = factorial(c - j) / (factorial(i - 1)*factorial(c - j - i + 1));
			}

			int sig = pow(-1, i + j + c + 1);
			MV(i - 1, j - 1) = sig*bin*mult;
		}
	}

	set_MV(MV);
}

int ITSurface::factorial(int n)
{
	int retval = 1;
	for (int i = n; i > 1; --i)
	{
		retval *= i;
	}
	return retval;
}

void ITSurface::computeURange()
{
	// This method constructs a vector of values of u at which the interpolated points will be computed.
	std::vector <float> *ur = new std::vector<float>;
	float du = 1.0f / (get_NoOfInterpolatedPointsU() - 1);

	for (int i = 0; i < get_NoOfInterpolatedPointsU(); i++)
	{
		ur->push_back(i*du);
	}

	set_URange(ur);
}

void ITSurface::computeVRange()
{
	// This method constructs a vector of values of v at which the interpolated points will be computed.
	std::vector <float> *vr = new std::vector<float>;
	float dv = 1.0f / (get_NoOfInterpolatedPointsV() - 1);

	for (int i = 0; i < get_NoOfInterpolatedPointsV(); i++)
	{
		vr->push_back(i*dv);
	}

	set_VRange(vr);
}

void ITSurface::computeUPowerMatrix()
{
	int r = get_MyControlPoints()->size(); // Checked
	int n = r - 1; // Checked

	Eigen::MatrixXd upm = Eigen::MatrixXd::Constant(get_NoOfInterpolatedPointsU(), r, 1.0);

	for (int i = 0; i < r; i++) // Columns
	{
		for (int j = 0; j < get_NoOfInterpolatedPointsU(); j++) // Rows
		{
			upm(j, n - i) = pow(get_URange()->at(j), i);
		}
	}

	set_UPowerMatrix(upm);
}

void ITSurface::computeVPowerMatrix()
{
	int c = get_MyControlPoints()->at(0).size(); // Checked
	int m = c - 1; // Checked

	Eigen::MatrixXd vpm = Eigen::MatrixXd::Constant(get_NoOfInterpolatedPointsV(), c, 1.0);

	for (int i = 0; i < c; i++) // Columns
	{
		for (int j = 0; j < get_NoOfInterpolatedPointsV(); j++) // Rows
		{
			vpm(j, m - i) = pow(get_VRange()->at(j), i);
		}
	}
	set_VPowerMatrix(vpm);
}

void ITSurface::computeInterpolatedPoints()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside computeInterpolatedPoints");

	// First find my index in the project get_MySurfaces array;
	int k;
	for (int n = 0; n < project->get_MySurfaces()->size(); n++)
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
		for (int i = 0; i < get_MyInterpolatedPoints()->size(); i++)
		{
			for (int j = 0; j < get_MyInterpolatedPoints()->at(0).size(); j++)
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
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			PX(i, j) = get_MyControlPoints()->at(i).at(j)->get_X();
			PY(i, j) = get_MyControlPoints()->at(i).at(j)->get_Y();
			PZ(i, j) = get_MyControlPoints()->at(i).at(j)->get_Z();
		}
	}

	// Compute the interpolated coordinates.
	Eigen::MatrixXd xb = get_UPowerMatrix() * get_MU() * PX * get_MV() * get_VPowerMatrix().transpose();
	Eigen::MatrixXd yb = get_UPowerMatrix() * get_MU() * PY * get_MV() * get_VPowerMatrix().transpose();
	Eigen::MatrixXd zb = get_UPowerMatrix() * get_MU() * PZ * get_MV() * get_VPowerMatrix().transpose();

	// Instanciate the vector of vectors of interpolated ITPoints.
	for (int i = 0; i < get_NoOfInterpolatedPointsU(); i++)
	{
		std::vector <ITPoint *> v_dummy_points;
		std::vector <ITPoint *> v_dummy_normals;

		for (int j = 0; j < get_NoOfInterpolatedPointsV(); j++)
		{
			// Create the interpolated points that will be used for adjustments.
			ITPoint *p = new ITPoint(xb(i, j), yb(i, j), zb(i, j));

			p->set_I(i);
			p->set_J(j);
			p->set_K(k);

			float du = 1.0f / (get_NoOfInterpolatedPointsU() - 1);
			float dv = 1.0f / (get_NoOfInterpolatedPointsV() - 1);

			p->set_U((float)i*du);
			p->set_V((float)j*dv);

			v_dummy_points.push_back(p);

			// Compute normal vector.
			ITPoint *n = new ITPoint(0.0f, 0.0f, 0.0f);
			if ((i < get_NoOfInterpolatedPointsU() - 1) && (j < get_NoOfInterpolatedPointsV() - 1))
			{
				float dxu = xb(i + 1, j) - xb(i, j);
				float dyu = yb(i + 1, j) - yb(i, j);
				float dzu = zb(i + 1, j) - zb(i, j);

				float dxv = xb(i, j + 1) - xb(i, j);
				float dyv = yb(i, j + 1) - yb(i, j);
				float dzv = zb(i, j + 1) - zb(i, j);

				// Do cross product.
				float nx = dyu*dzv - dyv*dzu;
				float ny = dzu*dxv - dxu*dzv;
				float nz = dxu*dyv - dyu*dxv;

				n->set_X(nx);
				n->set_Y(ny);
				n->set_Z(nz);
				n->normalize();
			}
			else if ((i == get_NoOfInterpolatedPointsU() - 1) && (j < get_NoOfInterpolatedPointsV() - 1))
			{
				float dxu = xb(i, j + 1) - xb(i, j);
				float dyu = yb(i, j + 1) - yb(i, j);
				float dzu = zb(i, j + 1) - zb(i, j);

				float dxv = xb(i - 1, j) - xb(i, j);
				float dyv = yb(i - 1, j) - yb(i, j);
				float dzv = zb(i - 1, j) - zb(i, j);

				// Do cross product.
				float nx = dyu*dzv - dyv*dzu;
				float ny = dzu*dxv - dxu*dzv;
				float nz = dxu*dyv - dyu*dxv;

				n->set_X(nx);
				n->set_Y(ny);
				n->set_Z(nz);
				n->normalize();
			}
			else if ((i < get_NoOfInterpolatedPointsU() - 1) && (j == get_NoOfInterpolatedPointsV() - 1))
			{
				float dxu = xb(i + 1, j) - xb(i, j);
				float dyu = yb(i + 1, j) - yb(i, j);
				float dzu = zb(i + 1, j) - zb(i, j);

				float dxv = xb(i, j - 1) - xb(i, j);
				float dyv = yb(i, j - 1) - yb(i, j);
				float dzv = zb(i, j - 1) - zb(i, j);

				// Do cross product.
				float nx = dyu*dzv - dyv*dzu;
				float ny = dzu*dxv - dxu*dzv;
				float nz = dxu*dyv - dyu*dxv;

				n->set_X(nx);
				n->set_Y(ny);
				n->set_Z(nz);
				n->normalize();
			}
			else
			{
				float dxu = xb(i - 1, j) - xb(i, j);
				float dyu = yb(i - 1, j) - yb(i, j);
				float dzu = zb(i - 1, j) - zb(i, j);

				float dxv = xb(i, j - 1) - xb(i, j);
				float dyv = yb(i, j - 1) - yb(i, j);
				float dzv = zb(i, j - 1) - zb(i, j);

				// Do cross product.
				float nx = dyu*dzv - dyv*dzu;
				float ny = dzu*dxv - dxu*dzv;
				float nz = dxu*dyv - dyu*dxv;

				n->set_X(nx);
				n->set_Y(ny);
				n->set_Z(nz);
				n->normalize();
			}

			v_dummy_normals.push_back(n);
		}

		// Fill the vectors of vectors.
		_MyInterpolatedPoints->push_back(v_dummy_points);
		_MyInterpolatedNormals->push_back(v_dummy_normals);
	}
}

void ITSurface::instanciateMyPanels(int mySurfaceIndex)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside instanciateMyPanels.");

	// Empty any existing panels.
	// There will be existing panels when instanciateMyPanels is called from Flexit::on_actionReset_triggered().
	if (!_MyPanels->empty())
	{
		int m = get_MyPanels()->size();
		for (int i = 0; i < m; i++)
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside instanciateMyPanels. i: %i", i);
			int n = get_MyPanels()->at(i).size();
			for (int j = 0; j < n; j++)
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside instanciateMyPanels. j: %i", j);
				delete get_MyPanels()->at(i).at(j);
			}
		}

		for (int i = 0; i < m; i++)
		{
			get_MyPanels()->erase(get_MyPanels()->begin());
		}

		get_MyPanels()->clear();
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside instanciateMyPanels 2.");

	// ===================================================================================
	// Create the new panel objects and push them into _MyPanels vector of vectors.
	// Note that instanciation of ITPanels implicitly instanciates 4 ITVortex objects.
	// ===================================================================================
	for (int i = 0; i < get_NoOfInterpolatedPointsU() - 1; i++)
	{
		std::vector <ITPanel*> dummy_v;
		for (int j = 0; j < get_NoOfInterpolatedPointsV() - 1; j++)
		{
			// Go around the panel in anti-clockwise direction.
			// Note that a single ITPoint object gets pointed to by 
			// the surface interpolated points vector AND the surface panels vector.
			// This means that when we propagate the interpolated points, 
			// the panel vertices SHOULD aautomatically update.
			ITPanel *p = new ITPanel(get_MyInterpolatedPoints()->at(i).at(j + 1),
				get_MyInterpolatedPoints()->at(i).at(j),
				get_MyInterpolatedPoints()->at(i + 1).at(j),
				get_MyInterpolatedPoints()->at(i + 1).at(j + 1),
				this, 0);
			p->set_I(i);
			p->set_J(j);
			p->set_K(mySurfaceIndex);

			// Decide whether this panel is COMPLETELY inside the fuselage.
			// Start by finding the vertex with the lagest absolute value of y.
			float largestY = 0.0;
			if (abs(get_MyInterpolatedPoints()->at(i).at(j + 1)->get_Y()) > largestY)
			{
				largestY = abs(get_MyInterpolatedPoints()->at(i).at(j + 1)->get_Y());
			}
			if (abs(get_MyInterpolatedPoints()->at(i).at(j)->get_Y()) > largestY)
			{
				largestY = abs(get_MyInterpolatedPoints()->at(i).at(j)->get_Y());
			}
			if (abs(get_MyInterpolatedPoints()->at(i + 1).at(j)->get_Y()) > largestY)
			{
				largestY = abs(get_MyInterpolatedPoints()->at(i + 1).at(j)->get_Y());
			}
			if (abs(get_MyInterpolatedPoints()->at(i + 1).at(j + 1)->get_Y()) > largestY)
			{
				largestY = abs(get_MyInterpolatedPoints()->at(i + 1).at(j + 1)->get_Y());
			}

			if (largestY < get_FuselageRadius())
			{
				p->set_IgnoreForces(true);
			}

			dummy_v.push_back(p);
		}
		_MyPanels->push_back(dummy_v);
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "End of instanciateMyPanels.");
}

void ITSurface::computeNetTrajectoryPointsAtFrame(int myFrameNumber, int mySurfaceIndex, ITPoint* translationPoint, ITPoint* rotationPoint)
{
	// This method computes the translation and rotation rigid body translation - taking into account the surface hierarchy tree.
	if ((project->get_IsSurfaceHierarchy()) && (get_MyAncestorSurfaceIndices()->size() > 0))
	{
		// Walk up the surface hierarchy accummulating transformations.
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "%i ", mySurfaceIndex);

		// Dummy points used for intermediate calculations.
		ITPoint *currentTranslationPoint = new ITPoint(0.0, 0.0, 0.0);
		ITPoint *currentRotationPoint = new ITPoint(0.0, 0.0, 0.0);
		ITPoint *cummulativeTranslationPoint = new ITPoint(0.0, 0.0, 0.0);

		float cumAlpha = 0.0;
		float cumBeta = 0.0;
		float cumGamma = 0.0;

		// The cummulative rotation matrix (initialized to the identity).
		float R[9] = { 1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, 0.0, 1.0 };

		float R_gamma_beta[9] = { 0.0 };

		for (int k = 0; k < get_MyAncestorSurfaceIndices()->size(); k++) // Loop from oldest ancestor to newest.
		{
			// Get the current ancestor index - starting with the oldest (root) surface.
			int ancestorIndex = get_MyAncestorSurfaceIndices()->at(k);

			// Compute the angles and the translation due to the current trajectory in the hierarchy.
			computeTrajectoryPointsAtFrame(myFrameNumber, ancestorIndex, currentTranslationPoint, currentRotationPoint);

			// Store the current (unrotated) translation as a C array.
			float T[3] = { currentTranslationPoint->get_X(), currentTranslationPoint->get_Y(), currentTranslationPoint->get_Z() };
			float V[3] = { 0.0 };

			// Rotate the current translation by the angles accummulated so far.
			ITPhysics::multiplyArbitraryMatrices(3, 3, 'R', R, 3, 1, 'R', T, V);

			// Add the currently rotated translation to the previous accummulated translation.
			cummulativeTranslationPoint->set_X(cummulativeTranslationPoint->get_X() + V[0]);
			cummulativeTranslationPoint->set_Y(cummulativeTranslationPoint->get_Y() + V[1]);
			cummulativeTranslationPoint->set_Z(cummulativeTranslationPoint->get_Z() + V[2]);

			// Debugging - print the cummulative translation for surface 1. This should equal the translation of surface 0.
			if (mySurfaceIndex == 2)
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "k: %i, cummulativeTranslationPoint x: %f, y: %f, z: %f", k, 
					cummulativeTranslationPoint->get_X(), cummulativeTranslationPoint->get_Y(), cummulativeTranslationPoint->get_Z());
			}

			// Now compute the rotation matrix ready to be used in the next pass through this loop.
			// Accummulate the angles.
			cumAlpha = cumAlpha + currentRotationPoint->get_X();
			cumBeta = cumBeta + currentRotationPoint->get_Y();
			cumGamma = cumGamma + currentRotationPoint->get_Z();

			// Form the accummulated angle rotation matrices (ready to adjust the next ancestor trajectory).
			// Roll.
			float Ralpha[9] = { 1.0, 0.0, 0.0,
				0.0, cos(cumAlpha), -sin(cumAlpha),
				0.0, sin(cumAlpha), cos(cumAlpha) };
			// Pitch.
			float Rbeta[9] = { cos(cumBeta), 0.0, sin(cumBeta),
				0.0, 1.0, 0.0,
				-sin(cumBeta), 0.0, cos(cumBeta) };
			// Yaw.
			float Rgamma[9] = { cos(cumGamma), -sin(cumGamma), 0.0,
				sin(cumGamma), cos(cumGamma), 0.0,
				0.0, 0.0, 1.0 };

			// Do two matrix multiplications to find the composite rotation matrix R.
			ITPhysics::multiplyArbitraryMatrices(3, 3, 'R', Rgamma, 3, 3, 'R', Rbeta, R_gamma_beta);
			ITPhysics::multiplyArbitraryMatrices(3, 3, 'R', R_gamma_beta, 3, 3, 'R', Ralpha, R);

			// Print the matrices to check them.
			ITPhysics::printThreeByThreeMatrix(3, 3, 'R', Rgamma, "Rgamma", 12);
			ITPhysics::printThreeByThreeMatrix(3, 3, 'R', Rbeta, "Rbeta", 12);
			ITPhysics::printThreeByThreeMatrix(3, 3, 'R', Ralpha, "Ralpha", 12);
			ITPhysics::printThreeByThreeMatrix(3, 3, 'R', R, "R", 12);

		} // End of walk up the heirarchy chain.

		delete currentTranslationPoint;
		delete currentRotationPoint;

		// Get the rotation and translation for "this" surface in isolation.
		ITPoint *finalTranslationPoint = new ITPoint(0.0, 0.0, 0.0);
		ITPoint *finalRotationPoint = new ITPoint(0.0, 0.0, 0.0);
		computeTrajectoryPointsAtFrame(myFrameNumber, mySurfaceIndex, finalTranslationPoint, finalRotationPoint);

		// Debugging - print the cummulative translation for surface 1. This should equal the translation of surface 0.
		if (mySurfaceIndex == 2)
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "cumGamma: %f, finalTranslationPoint x: %f, y: %f, z: %f", cumGamma, 
				finalTranslationPoint->get_X(), finalTranslationPoint->get_Y(), finalTranslationPoint->get_Z());
		}

		// Rotate the final (little) translation of "this" surface accordinng to the final cummulative angles.
		float T[3] = { finalTranslationPoint->get_X(), finalTranslationPoint->get_Y(), finalTranslationPoint->get_Z() };
		float Tf[3] = { 0.0 };

		ITPhysics::multiplyArbitraryMatrices(3, 3, 'R', R, 3, 1, 'R', T, Tf);

		// Debugging - print the cummulative translation for surface 1. This should equal the translation of surface 0.
		if (mySurfaceIndex == 2)
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Tf x: %f, y: %f, z: %f", Tf[0], Tf[1], Tf[2]);
		}

		translationPoint->set_X(cummulativeTranslationPoint->get_X() + Tf[0]);
		translationPoint->set_Y(cummulativeTranslationPoint->get_Y() + Tf[1]);
		translationPoint->set_Z(cummulativeTranslationPoint->get_Z() + Tf[2]);

		// Debugging - print the cummulative translation for surface 1. This should equal the translation of surface 0.
		if (mySurfaceIndex == 2)
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "translationPoint x: %f, y: %f, z: %f", translationPoint->get_X(), translationPoint->get_Y(), translationPoint->get_Z());
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, " ");
		}

		rotationPoint->set_X(cumAlpha + finalRotationPoint->get_X());
		rotationPoint->set_Y(cumBeta + finalRotationPoint->get_Y());
		rotationPoint->set_Z(cumGamma + finalRotationPoint->get_Z());

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "rotationPoint alpha: %f, beta: %f, gamma: %f", rotationPoint->get_X(), rotationPoint->get_Y(), rotationPoint->get_Z());

		// Delete temporary objects.
		delete finalTranslationPoint;
		delete finalRotationPoint;
		delete cummulativeTranslationPoint;

	}
	else
	{
		// Do a simple calculation without walking up the surface hierarchy.

		// 20161006 We believe it is these calls that takes a lot of time, so we are instrumenting it.
		//LARGE_INTEGER t1, t2, frequency;
		//double elapsedTime;
		//QueryPerformanceFrequency(&frequency);
		//QueryPerformanceCounter(&t1);

		computeTrajectoryPointsAtFrame(myFrameNumber, mySurfaceIndex, translationPoint, rotationPoint);

		//QueryPerformanceCounter(&t2);
		//elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
		//project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Time for computeTrajectoryPointsAtFrame at frame %i is %f, msecs", FrameNumber, elapsedTime);
	}
}

void ITSurface::computeTrajectoryPointsAtFrame(int myFrameNumber, int mySurfaceIndex, ITPoint* translationPoint, ITPoint* rotationPoint)
{
	// Revision history: 20160409: Complete re-write to use new and correct formulation of trajectory curves.
	//					 20161006: Huge improvement by setting T = (float)deltaFrames/(float)noOfFramesInCurrentSegment immediately and avoiding iteration for T.

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside computeTrajectoryPointsAtFrame");

	// Find kStar.
	// Loop through the segments of the curve to find kStar, 
	// the index of the segment in which myFrameNumber lies.
	int kStar = 0;

	// Test if kStar is in fact smaller.
	for (int k = 0; k < get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size(); k++)
	{
		if (myFrameNumber < get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(k)->get_EndKeyFrame())
		{
			// FrameNumber is strictly less than the end key frame of the k-th segment,
			// so set kStar and then break.
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "k: %i", k);

			kStar = k;

			break;
		}
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "computeTrajectoryPointsAtFrame kStar: %i", kStar);

	// deltaFrames is the number of frames between the start of segment kStar and the current FrameNumber (the value could be 0).
	int deltaFrames = myFrameNumber - get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(kStar)->get_StartKeyFrame();

	// noOfFramesInCurrentSegment is the total number of frames in segment kStar.
	int noOfFramesInCurrentSegment = get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(kStar)->get_EndKeyFrame() 
		- get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(kStar)->get_StartKeyFrame();

	// Note that project->get_IsConstantSpeedTrajectories() is not used.
	// We're going to compute the natural parametrization.
	// Loop along the kStar segment of each curve and find the parameter value of the sample immewdiately after th current frame.
	ITTrajectoryCurveSegment* xSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(mySurfaceIndex)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(kStar));
	ITTrajectoryCurveSegment* ySegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(mySurfaceIndex)->get_MyTrajectoryCurves()->at(1)->get_MyTrajectoryCurveSegments()->at(kStar));
	ITTrajectoryCurveSegment* zSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(mySurfaceIndex)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(kStar));
	ITTrajectoryCurveSegment* rSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(mySurfaceIndex)->get_MyTrajectoryCurves()->at(3)->get_MyTrajectoryCurveSegments()->at(kStar));
	ITTrajectoryCurveSegment* pSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(mySurfaceIndex)->get_MyTrajectoryCurves()->at(4)->get_MyTrajectoryCurveSegments()->at(kStar));
	ITTrajectoryCurveSegment* yawSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(mySurfaceIndex)->get_MyTrajectoryCurves()->at(5)->get_MyTrajectoryCurveSegments()->at(kStar));

	float Tx = 0.0;
	float Ty = 0.0;
	float Tz = 0.0;
	float Tr = 0.0;
	float Tp = 0.0;
	float Tyaw = 0.0;

	if (project->get_IsConstantSpeedTrajectories())
	{
		// Now do some linear interpolation.
		float x0, x1, xx, y0, y1;
		// x
		x0 = (float)(xSegment->get_StartKeyFrame());
		x1 = (float)(xSegment->get_EndKeyFrame());
		xx = (float)(myFrameNumber);
		y0 = xSegment->get_P0_p()->get_X();
		y1 = xSegment->get_P1_p()->get_X();
		float x = y0 + ((xx - x0) / (x1 - x0))*(y1 - y0);

		// y
		x0 = (float)(ySegment->get_StartKeyFrame());
		x1 = (float)(ySegment->get_EndKeyFrame());
		xx = (float)(myFrameNumber);
		y0 = ySegment->get_P0_p()->get_X();
		y1 = ySegment->get_P1_p()->get_X();
		float y = y0 + ((xx - x0) / (x1 - x0))*(y1 - y0);

		// z
		x0 = (float)(zSegment->get_StartKeyFrame());
		x1 = (float)(zSegment->get_EndKeyFrame());
		xx = (float)(myFrameNumber);
		y0 = zSegment->get_P0_p()->get_X();
		y1 = zSegment->get_P1_p()->get_X();
		float z = y0 + ((xx - x0) / (x1 - x0))*(y1 - y0);

		// r
		x0 = (float)(rSegment->get_StartKeyFrame());
		x1 = (float)(rSegment->get_EndKeyFrame());
		xx = (float)(myFrameNumber);
		y0 = rSegment->get_P0_p()->get_X();
		y1 = rSegment->get_P1_p()->get_X();
		float r = y0 + ((xx - x0) / (x1 - x0))*(y1 - y0);

		// p
		x0 = (float)(pSegment->get_StartKeyFrame());
		x1 = (float)(pSegment->get_EndKeyFrame());
		xx = (float)(myFrameNumber);
		y0 = pSegment->get_P0_p()->get_X();
		y1 = pSegment->get_P1_p()->get_X();
		float p = y0 + ((xx - x0) / (x1 - x0))*(y1 - y0);

		// yaw
		x0 = (float)(yawSegment->get_StartKeyFrame());
		x1 = (float)(yawSegment->get_EndKeyFrame());
		xx = (float)(myFrameNumber);
		y0 = yawSegment->get_P0_p()->get_X();
		y1 = yawSegment->get_P1_p()->get_X();
		float yaw = y0 + ((xx - x0) / (x1 - x0))*(y1 - y0);

		// Put the data in translationPoint and ratationPoint
		translationPoint->set_X(x);
		translationPoint->set_Y(y);
		translationPoint->set_Z(z);

		rotationPoint->set_X(r);
		rotationPoint->set_Y(p);
		rotationPoint->set_Z(yaw);
	}
	else
	{
		Tx = (float)deltaFrames / (float)noOfFramesInCurrentSegment;
		Ty = (float)deltaFrames / (float)noOfFramesInCurrentSegment;
		Tz = (float)deltaFrames / (float)noOfFramesInCurrentSegment;
		Tr = (float)deltaFrames / (float)noOfFramesInCurrentSegment;
		Tp = (float)deltaFrames / (float)noOfFramesInCurrentSegment;
		Tyaw = (float)deltaFrames / (float)noOfFramesInCurrentSegment;

		ITPoint * px = xSegment->evaluatePointAtParameterValue(Tx);
		ITPoint * py = ySegment->evaluatePointAtParameterValue(Ty);
		ITPoint * pz = zSegment->evaluatePointAtParameterValue(Tz);
		ITPoint * pr = rSegment->evaluatePointAtParameterValue(Tr);
		ITPoint * pp = pSegment->evaluatePointAtParameterValue(Tp);
		ITPoint * pyaw = yawSegment->evaluatePointAtParameterValue(Tyaw);

		// Put the data in translationPoint and ratationPoint
		translationPoint->set_X(px->get_Y());
		translationPoint->set_Y(py->get_Y());
		translationPoint->set_Z(pz->get_Y());

		rotationPoint->set_X(pr->get_Y());
		rotationPoint->set_Y(pp->get_Y());
		rotationPoint->set_Z(pyaw->get_Y());

		delete px;
		delete py;
		delete pz;
		delete pr;
		delete pp;
		delete pyaw;
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Returning from computeTrajectoryPointsAtFrame");
}

void ITSurface::propagateGeometry(int k)
{
	// This method computes new surface, panel and vortex segment geometry for the current FrameNumber.
	// Propagate control points first.
	// Get the "base" point for rotations.
	ITPoint *cp = project->get_MyBaseSurfaces()->at(k)->get_MyCentreOfRotationPoint();

	// Find the points on the translation and rotation trajectory curves corresponding to this frame.
	// Instanciate ITPoint objects that will contain the coordinates of the
	// current point in rigid translation and rotation space for this surface.
	ITPoint *translationPoint = new ITPoint(0.0, 0.0, 0.0);
	ITPoint *rotationPoint = new ITPoint(0.0, 0.0, 0.0);

	// Actually calculate the points on the translation and rotation trajectory curves that are relevant to the current FrameNumber.
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "About to call computeNetTrajectoryPointsAtFrame for the current frame. FrameNumber = %i", FrameNumber);
	computeNetTrajectoryPointsAtFrame(FrameNumber, k, translationPoint, rotationPoint);

	// Update the instance variables for this surface.
	get_MyCurrentTranslationPoint()->set_X(translationPoint->get_X());
	get_MyCurrentTranslationPoint()->set_Y(translationPoint->get_Y());
	get_MyCurrentTranslationPoint()->set_Z(translationPoint->get_Z());

	get_MyCurrentRotationPoint()->set_X(rotationPoint->get_X());
	get_MyCurrentRotationPoint()->set_Y(rotationPoint->get_Y());
	get_MyCurrentRotationPoint()->set_Z(rotationPoint->get_Z());

	// Actually calculate the points on the translation and rotation trajectory curves that are relevant to the previous FrameNumber.
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "About to call computeNetTrajectoryPointsAtFrame for the previous frame. FrameNumber = %i", FrameNumber);

	if (FrameNumber > 0)
	{
		computeNetTrajectoryPointsAtFrame(FrameNumber - 1, k, translationPoint, rotationPoint);
	}
	else
	{
		// We are at FrameNumber = 0 so initialize the previous points to the same as those for FrameNumber=1.
		// Setting the previous trajectory points identical to those for ther current frame number when FrameNumber = 0 will ensure that the initial panel velocities are zero.
		// This will reduce the violent starting vortex.
		computeNetTrajectoryPointsAtFrame(FrameNumber, k, translationPoint, rotationPoint);
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "translation x: %f, translation y: %f, translation z: %f. FrameNumber = %i", 
		translationPoint->get_X(), translationPoint->get_Y(), translationPoint->get_Z(), FrameNumber);

	// Update the instance variables for this surface.
	get_MyPreviousTranslationPoint()->set_X(translationPoint->get_X());
	get_MyPreviousTranslationPoint()->set_Y(translationPoint->get_Y());
	get_MyPreviousTranslationPoint()->set_Z(translationPoint->get_Z());

	get_MyPreviousRotationPoint()->set_X(rotationPoint->get_X());
	get_MyPreviousRotationPoint()->set_Y(rotationPoint->get_Y());
	get_MyPreviousRotationPoint()->set_Z(rotationPoint->get_Z());

	// free the memory.
	delete translationPoint;
	delete rotationPoint;

	// ========================================================================================================
	// We have just finished computing the translation and rotation points for the current and previous points.
	// ========================================================================================================

	// Actually do the transformation of the surface control points.
	// Instanciate a temporary local ITPoint object to be used for intermediate calculations.
	ITPoint *currentPoint = new ITPoint(0.0f, 0.0f, 0.0f);

	// Ints i and j count through the rows and columns of the vector of vectors of control points of this surface.
	for (int i = 0; i < get_MyControlPoints()->size(); i++)
	{
		for (int j = 0; j < get_MyControlPoints()->at(i).size(); j++)
		{
			// Assign the coordinates of the current control point base position to the currentPoint temporary object.
			bool IsDoEB = true; // Temporarily disable propagatiion of deformations while we debug the code.

			if ((IsModeEulerBernoulli) && (IsDoEB))
			{
				// Include deformation.
				currentPoint->set_X(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X() 
					+ project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_W()->get_X());
				currentPoint->set_Y(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y() 
					+ project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_W()->get_Y());
				currentPoint->set_Z(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z() 
					+ project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_W()->get_Z());
			}
			else
			{
				// Ignore deformation.
				currentPoint->set_X(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X());
				currentPoint->set_Y(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y());
				currentPoint->set_Z(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z());
			}

			currentPoint->propagateMeWithMorph(cp, get_MyCurrentRotationPoint(), get_MyCurrentTranslationPoint(), get_IsMorph(), get_MorphStartFrame(), get_MorphEndFrame(), get_MorphType());
			// Re-assign control point coordinates.
			get_MyControlPoints()->at(i).at(j)->set_X(currentPoint->get_X());
			get_MyControlPoints()->at(i).at(j)->set_Y(currentPoint->get_Y());
			get_MyControlPoints()->at(i).at(j)->set_Z(currentPoint->get_Z());
		}
	}

	// Delete the temporary object.
	delete currentPoint;

	// Now compute newly propagated interpolated points.
	manageComputationOfInterpolatedPoints();

	// Propagate the underlying panel geometry.
	managePropagationOfPanels();

	return;
}

void ITSurface::managePropagationOfPanels()
{
	// Here the ITPanel perimeter point objects were the interpolated points.
	// The old interpolated point objects were deleted in computeInterpolatedPoints, 
	// so here we need to point the ITPanel vertices to the new interpolated points.

	int m = get_MyPanels()->size();
	for (int i = 0; i < m; i++)
	{
		int n = get_MyPanels()->at(0).size();
		for (int j = 0; j < n; j++)
		{
			ITPanel *p = get_MyPanels()->at(i).at(j);

			// Copy interpolated data into ITPanel vertices.

			p->get_BottomLeftPoint()->set_X(get_MyInterpolatedPoints()->at(i).at(j + 1)->get_X());
			p->get_BottomLeftPoint()->set_Y(get_MyInterpolatedPoints()->at(i).at(j + 1)->get_Y());
			p->get_BottomLeftPoint()->set_Z(get_MyInterpolatedPoints()->at(i).at(j + 1)->get_Z());

			p->get_BottomRightPoint()->set_X(get_MyInterpolatedPoints()->at(i).at(j)->get_X());
			p->get_BottomRightPoint()->set_Y(get_MyInterpolatedPoints()->at(i).at(j)->get_Y());
			p->get_BottomRightPoint()->set_Z(get_MyInterpolatedPoints()->at(i).at(j)->get_Z());

			p->get_TopRightPoint()->set_X(get_MyInterpolatedPoints()->at(i + 1).at(j)->get_X());
			p->get_TopRightPoint()->set_Y(get_MyInterpolatedPoints()->at(i + 1).at(j)->get_Y());
			p->get_TopRightPoint()->set_Z(get_MyInterpolatedPoints()->at(i + 1).at(j)->get_Z());

			p->get_TopLeftPoint()->set_X(get_MyInterpolatedPoints()->at(i + 1).at(j + 1)->get_X());
			p->get_TopLeftPoint()->set_Y(get_MyInterpolatedPoints()->at(i + 1).at(j + 1)->get_Y());
			p->get_TopLeftPoint()->set_Z(get_MyInterpolatedPoints()->at(i + 1).at(j + 1)->get_Z());

			// If IsPistolesiPanelOffset is true then update the locations of the panel vertices.
			ITPoint *botL = p->get_BottomLeftPoint();
			ITPoint *botR = p->get_BottomRightPoint();
			ITPoint *topR = p->get_TopRightPoint();
			ITPoint *topL = p->get_TopLeftPoint();

			if (get_IsPistolesiPanelOffset())
			{
				// Do Pistolesi computations for a bound panel.
				ITPoint *botL = new ITPoint(0.0, 0.0, 0.0);
				ITPoint *botR = new ITPoint(0.0, 0.0, 0.0);
				ITPoint *topR = new ITPoint(0.0, 0.0, 0.0);
				ITPoint *topL = new ITPoint(0.0, 0.0, 0.0);

				// Start with bottom left.
				botL->set_X(p->get_BottomLeftPoint()->get_X() + get_PistolesiPanelOffsetFraction()*(p->get_BottomLeftPoint()->get_X() - p->get_BottomRightPoint()->get_X()));
				botL->set_Y(p->get_BottomLeftPoint()->get_Y() + get_PistolesiPanelOffsetFraction()*(p->get_BottomLeftPoint()->get_Y() - p->get_BottomRightPoint()->get_Y()));
				botL->set_Z(p->get_BottomLeftPoint()->get_Z() + get_PistolesiPanelOffsetFraction()*(p->get_BottomLeftPoint()->get_Z() - p->get_BottomRightPoint()->get_Z()));

				// Bottom right.
				botR->set_X(p->get_BottomRightPoint()->get_X() + get_PistolesiPanelOffsetFraction()*(p->get_BottomLeftPoint()->get_X() - p->get_BottomRightPoint()->get_X()));
				botR->set_Y(p->get_BottomRightPoint()->get_Y() + get_PistolesiPanelOffsetFraction()*(p->get_BottomLeftPoint()->get_Y() - p->get_BottomRightPoint()->get_Y()));
				botR->set_Z(p->get_BottomRightPoint()->get_Z() + get_PistolesiPanelOffsetFraction()*(p->get_BottomLeftPoint()->get_Z() - p->get_BottomRightPoint()->get_Z()));

				// Top right.
				topR->set_X(p->get_TopRightPoint()->get_X() + get_PistolesiPanelOffsetFraction()*(p->get_TopLeftPoint()->get_X() - p->get_TopRightPoint()->get_X()));
				topR->set_Y(p->get_TopRightPoint()->get_Y() + get_PistolesiPanelOffsetFraction()*(p->get_TopLeftPoint()->get_Y() - p->get_TopRightPoint()->get_Y()));
				topR->set_Z(p->get_TopRightPoint()->get_Z() + get_PistolesiPanelOffsetFraction()*(p->get_TopLeftPoint()->get_Z() - p->get_TopRightPoint()->get_Z()));

				// Top left.
				topL->set_X(p->get_TopLeftPoint()->get_X() + get_PistolesiPanelOffsetFraction()*(p->get_TopLeftPoint()->get_X() - p->get_TopRightPoint()->get_X()));
				topL->set_Y(p->get_TopLeftPoint()->get_Y() + get_PistolesiPanelOffsetFraction()*(p->get_TopLeftPoint()->get_Y() - p->get_TopRightPoint()->get_Y()));
				topL->set_Z(p->get_TopLeftPoint()->get_Z() + get_PistolesiPanelOffsetFraction()*(p->get_TopLeftPoint()->get_Z() - p->get_TopRightPoint()->get_Z()));

				// We have the new coords, so set the instance variables of the panel vertices.
				p->get_BottomLeftPoint()->set_X(botL->get_X());
				p->get_BottomLeftPoint()->set_Y(botL->get_Y());
				p->get_BottomLeftPoint()->set_Z(botL->get_Z());

				p->get_BottomRightPoint()->set_X(botR->get_X());
				p->get_BottomRightPoint()->set_Y(botR->get_Y());
				p->get_BottomRightPoint()->set_Z(botR->get_Z());

				p->get_TopRightPoint()->set_X(topR->get_X());
				p->get_TopRightPoint()->set_Y(topR->get_Y());
				p->get_TopRightPoint()->set_Z(topR->get_Z());

				p->get_TopLeftPoint()->set_X(topL->get_X());
				p->get_TopLeftPoint()->set_Y(topL->get_Y());
				p->get_TopLeftPoint()->set_Z(topL->get_Z());

				delete botL;
				delete botR;
				delete topR;
				delete topL;
			}

			// Compute derived geometry.
			get_MyPanels()->at(i).at(j)->computeDiagonals();
			get_MyPanels()->at(i).at(j)->computeMidPoint();
			get_MyPanels()->at(i).at(j)->computeColocationPoint();
			get_MyPanels()->at(i).at(j)->computeNormal();
			get_MyPanels()->at(i).at(j)->computeArea();
		}
	}
}

void ITSurface::insertWakePanels()
{
	// This function is responsible for inserting new wake panels at the
	// trailing edge as the Flexit simulation progresses.

	// Note that the first index in the WakePanels vector of vectors is the generation,
	// and the second index is the row down the surface trailing edge (i).

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside ITSurface insertWakePanels");

	// Traverse the trailing edge panels, inserting Wake Panels that are put into the vector dummy_v.

	std::vector <ITWakePanel*> dummy_v; // A vector that will contain the wake panels created at this frame.

	// Loop down the trailing edge surface panels.
	for (int i = 0; i < get_MyPanels()->size(); i++)
	{
		if (FrameNumber == 0)
		{
			// 20161007: Modified to take account of Pistolesi offset.

			// We are on the first iteration, so add the first column of wake panels.
			float eps = 0.0f; // Make a tiny gap between the back of the surface and the new panel.

			// Go around the panel in anti-clockwise direction.

			// We are on the first Frame (when the Surface has not yet moved), so we artificially offset
			// the left (aft) edges of the wake panels in the "down-stream" direction.

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "                         1 BLx: %f", get_MyPanels()->at(i).back()->get_BottomLeftPoint()->get_X());

			ITPoint *bl = new ITPoint();
			bl->set_X(get_MyPanels()->at(i).back()->get_BottomLeftPoint()->get_X() - 1.0f);
			bl->set_Y(get_MyPanels()->at(i).back()->get_BottomLeftPoint()->get_Y());
			bl->set_Z(get_MyPanels()->at(i).back()->get_BottomLeftPoint()->get_Z());

			ITPoint *br = new ITPoint();
			br->set_X(get_MyPanels()->at(i).back()->get_BottomLeftPoint()->get_X() - eps);
			br->set_Y(get_MyPanels()->at(i).back()->get_BottomLeftPoint()->get_Y());
			br->set_Z(get_MyPanels()->at(i).back()->get_BottomLeftPoint()->get_Z());

			ITPoint *tr = new ITPoint();
			tr->set_X(get_MyPanels()->at(i).back()->get_TopLeftPoint()->get_X() - eps);
			tr->set_Y(get_MyPanels()->at(i).back()->get_TopLeftPoint()->get_Y());
			tr->set_Z(get_MyPanels()->at(i).back()->get_TopLeftPoint()->get_Z());

			ITPoint *tl = new ITPoint();
			tl->set_X(get_MyPanels()->at(i).back()->get_TopLeftPoint()->get_X() - 1.0f);
			tl->set_Y(get_MyPanels()->at(i).back()->get_TopLeftPoint()->get_Y());
			tl->set_Z(get_MyPanels()->at(i).back()->get_TopLeftPoint()->get_Z());

			// Instanciate the new wake panel object.
			ITWakePanel *p = new ITWakePanel(bl, br, tr, tl, this, 1);

			// Get a pointer to the surface panel that is shedding this new one.
			ITPanel* parentPanel = get_MyPanels()->at(i).back();

			// Set the vorticity of the new wake panel to be equal to the vorticity of the surface panel that is shedding this wake.
			p->set_MyVorticity(parentPanel->get_MyVorticity());

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "insertWakePanels. Parent Vorticity: %f,  Wake Vorticity: %f", parentPanel->get_MyVorticity(), p->get_MyVorticity());

			// Push the new wake panel into the vector of wake panels "created" at this computational frame.
			dummy_v.push_back(p);

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Row: %i", i);
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "First wake BL. x: %f, y: %f, z: %f, original z: %f", bl->get_X(), 
				bl->get_Y(), bl->get_Z(), get_MyInterpolatedPoints()->at(i).back()->get_Z());
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "First wake BR. x: %f, y: %f, z: %f, original z: %f", br->get_X(), 
				br->get_Y(), br->get_Z(), get_MyInterpolatedPoints()->at(i).back()->get_Z());
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "First wake TR. x: %f, y: %f, z: %f, original z: %f", tr->get_X(), 
				tr->get_Y(), tr->get_Z(), get_MyInterpolatedPoints()->at(i + 1).back()->get_Z());
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "First wake TL. x: %f, y: %f, z: %f, original z: %f", tl->get_X(), 
				tl->get_Y(), tl->get_Z(), get_MyInterpolatedPoints()->at(i + 1).back()->get_Z());

			// Delete the temp ITPoint objects.
			delete bl;
			delete br;
			delete tr;
			delete tl;
		}
		else
		{
			// =========================================================================
			// We are on a second- or greater wake generation, so no need to consider
			// semi-infinite vortices.
			// The main thing here is that the up-stream vertices of the new wake panels are
			// coincident with the (deformed) trailing edge panels of the surface.
			// =========================================================================

			// Check if we need to truncate the wake panels.
			if (project->get_MaxPropagationGeneration() > 0)
			{
				if (project->get_MaxPropagationGeneration() < get_MyWakePanels()->size())
				{
					// Delete old panels.
					int noOfGenrationsToDelete = get_MyWakePanels()->size() - project->get_MaxPropagationGeneration();
					for (int j = 0; j < get_MyWakePanels()->at(0).size(); j++)
					{
						delete get_MyWakePanels()->at(0).at(j);
					}
					get_MyWakePanels()->erase(get_MyWakePanels()->begin());
					get_MyWakePanels()->at(0).clear();
				}
			}

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "                          BLx: %f", get_MyPanels()->at(i).back()->get_BottomLeftPoint()->get_X());

			// Go around the panel in anti-clockwise direction.
			// The index ii is the index of the previous generation of wake panels (from the previous computation frame)
			int ii = get_MyWakePanels()->size() - 1;

			float eps = 0.0f; // Make a tiny gap between the back of the surface and the new panel.

			// Make the bottom left vertex of the new wake panel coincident with the bottom right
			// vertex of the wake panel shed from the trailing edge on the previoud computation frame (of the previous generation).
			ITPoint *bl = new ITPoint();
			bl->set_X(get_MyWakePanels()->at(ii).at(i)->get_BottomRightPoint()->get_X() + eps);
			bl->set_Y(get_MyWakePanels()->at(ii).at(i)->get_BottomRightPoint()->get_Y());
			bl->set_Z(get_MyWakePanels()->at(ii).at(i)->get_BottomRightPoint()->get_Z());

			// Make the bottom right vertex of the new wake panel coincident with the (deformed) bottom left
			// vertex of the bound panel on the trailing edge of the surface.
			ITPoint *br = new ITPoint();
			br->set_X(get_MyPanels()->at(i).back()->get_BottomLeftPoint()->get_X() - eps);
			br->set_Y(get_MyPanels()->at(i).back()->get_BottomLeftPoint()->get_Y());
			br->set_Z(get_MyPanels()->at(i).back()->get_BottomLeftPoint()->get_Z());

			// Make the top right vertex of the new wake panel coincident with the top right
			// vertex of the bound panel on the trailing edge of the surface.
			ITPoint *tr = new ITPoint();
			tr->set_X(get_MyPanels()->at(i).back()->get_TopLeftPoint()->get_X() - eps);
			tr->set_Y(get_MyPanels()->at(i).back()->get_TopLeftPoint()->get_Y());
			tr->set_Z(get_MyPanels()->at(i).back()->get_TopLeftPoint()->get_Z());

			// Make the top left vertex of the new wake panel coincident with the top right
			// vertex of the wake panel shed from the trailing edge on the previoud computation frame.
			ITPoint *tl = new ITPoint();
			tl->set_X(get_MyWakePanels()->at(ii).at(i)->get_TopRightPoint()->get_X() + eps);
			tl->set_Y(get_MyWakePanels()->at(ii).at(i)->get_TopRightPoint()->get_Y());
			tl->set_Z(get_MyWakePanels()->at(ii).at(i)->get_TopRightPoint()->get_Z());

			// Instanciate the new wake panel.
			ITWakePanel *p = new ITWakePanel(bl, br, tr, tl, this, 1);

			// Delete the temp ITPoint objects.
			delete bl;
			delete br;
			delete tr;
			delete tl;

			// Get a pointer to the surface panel that is shedding this new one.
			ITPanel* parentPanel = get_MyPanels()->at(i).at(get_NoOfInterpolatedPointsV() - 2);

			// Set the vorticity of the new wake panel.
			p->set_MyVorticity(parentPanel->get_MyVorticity());

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Row: %i", i);
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Bottom butt report 1. old x: %f, bl x: %f, p->bl x: %f",
				get_MyWakePanels()->at(ii).at(i)->get_BottomRightPoint()->get_X(), bl->get_X(), p->get_BottomLeftPoint()->get_X());
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Bottom butt report 1. old y: %f, bl y: %f, p->bl y: %f", 
				get_MyWakePanels()->at(ii).at(i)->get_BottomRightPoint()->get_Y(), bl->get_Y(), p->get_BottomLeftPoint()->get_Y());
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Bottom butt report 1. old z: %f, bl z: %f, p->bl z: %f", 
				get_MyWakePanels()->at(ii).at(i)->get_BottomRightPoint()->get_Z(), bl->get_Z(), p->get_BottomLeftPoint()->get_Z());

			// Push the new wake panel into the vector of wake panels "created" at this computational frame.
			dummy_v.push_back(p);

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "New column of %i wake panels has been created. Size: %i", dummy_v.size(), get_MyWakePanels()->size());
		}
	}

	// ========================================================================================
	// Add the new vector of wake panels of the current generation to the vector of
	// vectors of wake panels of this surface.
	// ========================================================================================
	get_MyWakePanels()->push_back(dummy_v);
}

void ITSurface::propagateWakeGeometryCuda(int k)
{
	// This method propagates the wake geometry.
	// It performs the updating of temporary wake segment vertex data velocities in a Cuda batch.

	// =========================================================================================
	// First determine the upper and lower bounds on the index of wake generation to be updated.
	int firstGeneration;
	int lastGeneration;

	if (project->get_MaxPropagationGeneration() > 0)
	{
		if (project->get_MaxPropagationGeneration() < project->get_MySurfaces()->at(k)->get_MyWakePanels()->size())
		{
			firstGeneration = project->get_MySurfaces()->at(k)->get_MyWakePanels()->size() - project->get_MaxPropagationGeneration();
			lastGeneration = project->get_MySurfaces()->at(k)->get_MyWakePanels()->size();
		}
		else
		{
			firstGeneration = 0;
			lastGeneration = project->get_MySurfaces()->at(k)->get_MyWakePanels()->size();
		}
	}
	else
	{
		firstGeneration = 0;
		lastGeneration = project->get_MySurfaces()->at(k)->get_MyWakePanels()->size();
	}

	// Define DELTA_T. It is used to multiply the local flow velocity magnitude to get propagation displacement.
	float DELTA_T = 1.0f / ((float)(project->get_FramesPerSecond()));

	// Vectors of vectors of ITPoints to provide temporary storage of wake panel vertex coordinates.
	std::vector < std::vector <ITPoint*> > *tempBottomLeft = new std::vector < std::vector <ITPoint*> >;
	std::vector < std::vector <ITPoint*> > *tempBottomRight = new std::vector < std::vector <ITPoint*> >;
	std::vector < std::vector <ITPoint*> > *tempTopRight = new std::vector < std::vector <ITPoint*> >;
	std::vector < std::vector <ITPoint*> > *tempTopLeft = new std::vector < std::vector <ITPoint*> >;

	// =========================================================================================
	// noOfSubjectPanels is the total number of panels in the project
	// (both bound panels on the surface and free panels in the wake, on all surfaces, including reflected ones).
	int noOfSubjectPanels = 0;

	for (int kk = 0; kk < project->get_MySurfaces()->size(); kk++) // Loop through all the surfaces.
	{
		// Add the number of bound surface panels to noOfSubjectPanels
		for (int ii = 0; ii < project->get_MySurfaces()->at(kk)->get_MyPanels()->size(); ii++)
		{
			for (int jj = 0; jj < project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).size(); jj++)
			{
				// Add surface subject panels.
				noOfSubjectPanels++;
			}
		}

		// Add the number of wake panels to noOfSubjectPanels
		for (int ii = firstGeneration; ii < lastGeneration; ii++) // Loop over generations.
		{
			for (int jj = 0; jj < project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).size(); jj++)
			{
				// Add wake subject panels.
				noOfSubjectPanels++;
			}
		}
	}

	// =========================================================================================
	// noOfVelocityPredictions is the number of points at which we require a prediction of the velocity.
	int noOfVelocityPredictions = 0; // The total number of wake adjustments at this generation t.
	// The number of vortices of any panel.
	int noOfVorticesPerPanel = 4;

	for (int i = firstGeneration; i < lastGeneration; i++) // Loop over generations.
	{
		for (int j = 0; j < get_MyWakePanels()->at(i).size(); j++) // Loop down trailing edge.
		{
			noOfVelocityPredictions = noOfVelocityPredictions + noOfVorticesPerPanel; // Add four vertices for each wake panel.
		}
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, 
		"========================================== Inside ITSurface propagateWakeGeometryCuda. DELTA_T: %f, noOfSubjectPanels: %i, noOfVelocityPredictions: %i", 
		DELTA_T, noOfSubjectPanels, noOfVelocityPredictions);

	// =========================================================================================
	// Set up allocation of arrays of panels and vertices in host memory.
	size_t sizeOfSubjectPanelsFloat = noOfSubjectPanels * sizeof(float); // Memory required for noOfPanels floats.
	size_t sizeOfSubjectPanelsInt = noOfSubjectPanels * sizeof(int); // Memory required for noOfPanels ints.
	size_t sizeVelocityPredictionsFloat = noOfVelocityPredictions * sizeof(float); // Memory required for noOfVelocityPredictions floats. This is for each (normalised) velocity component.

	// Allocate memory for Kernel input data on the host.
	// Coordinates of points at which velocities are to be computed.
	float *h_cp_x = (float *)malloc(sizeVelocityPredictionsFloat); // Allocate the host object-point x-coordinate.
	float *h_cp_y = (float *)malloc(sizeVelocityPredictionsFloat); // Allocate the host object-point y-coordinate.
	float *h_cp_z = (float *)malloc(sizeVelocityPredictionsFloat); // Allocate the host object-point z-coordinate.

	// Subject Vortex start point coordinates.
	float *h_vs_x = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex start point x-coord.
	float *h_vs_y = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex start point y-coord.
	float *h_vs_z = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex start point z-coord.

																					// Subject Vortex end point coordinates.
	float *h_ve_x = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex end point x-coord.
	float *h_ve_y = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex end point y-coord.
	float *h_ve_z = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex end point z-coord.

	// Allocate memory for vorticity values for each subject panel.
	float *h_vorticities = (float *)malloc(sizeOfSubjectPanelsFloat);

	// Allocate the host output vectors of velocity components for each object point.
	float *h_cp_vx = (float *)malloc(sizeVelocityPredictionsFloat);
	float *h_cp_vy = (float *)malloc(sizeVelocityPredictionsFloat);
	float *h_cp_vz = (float *)malloc(sizeVelocityPredictionsFloat);

	// Verify that allocations succeeded.
	if (h_cp_x == NULL
		|| h_cp_y == NULL
		|| h_cp_z == NULL
		|| h_vs_x == NULL
		|| h_vs_y == NULL
		|| h_vs_z == NULL
		|| h_ve_x == NULL
		|| h_ve_y == NULL
		|| h_ve_z == NULL
		|| h_vorticities == NULL
		|| h_cp_vx == NULL
		|| h_cp_vy == NULL
		|| h_cp_vz == NULL)
	{
		fprintf(stderr, "Failed to allocate host vectors!\n");
		exit(EXIT_FAILURE);
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Host memory allocated successfully.");

	// =========================================================================================
	// Now initialize the host array of object point velocity components (output).
	for (int i = 0; i < noOfVelocityPredictions; i++)
	{
		h_cp_vx[i] = 0.0f;
		h_cp_vy[i] = 0.0f;
		h_cp_vz[i] = 0.0f;
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Host object point velocities initialized successfully.");

	// =========================================================================================
	// Now initialize the host arrays of object point position
	// coordinates (the wake panel vertices).

	int objectPointIndex = 0; // DOM: Object point index.

	for (int ii = firstGeneration; ii < lastGeneration; ii++) // DOM: Loop through the generations of wake panels on this surface.
	{
		for (int jj = 0; jj < get_MyWakePanels()->at(ii).size(); jj++) // DOM: Loop through the wake panels on the ii-th generation on this surface.
		{
			// DOM: Assign object point coordinates.
			// Bottom Left.
			h_cp_x[objectPointIndex] = get_MyWakePanels()->at(ii).at(jj)->get_BottomLeftPoint()->get_X();
			h_cp_y[objectPointIndex] = get_MyWakePanels()->at(ii).at(jj)->get_BottomLeftPoint()->get_Y();
			h_cp_z[objectPointIndex] = get_MyWakePanels()->at(ii).at(jj)->get_BottomLeftPoint()->get_Z();

			objectPointIndex++;

			// Bottom Right.
			h_cp_x[objectPointIndex] = get_MyWakePanels()->at(ii).at(jj)->get_BottomRightPoint()->get_X();
			h_cp_y[objectPointIndex] = get_MyWakePanels()->at(ii).at(jj)->get_BottomRightPoint()->get_Y();
			h_cp_z[objectPointIndex] = get_MyWakePanels()->at(ii).at(jj)->get_BottomRightPoint()->get_Z();

			objectPointIndex++;

			// Top Right.
			h_cp_x[objectPointIndex] = get_MyWakePanels()->at(ii).at(jj)->get_TopRightPoint()->get_X();
			h_cp_y[objectPointIndex] = get_MyWakePanels()->at(ii).at(jj)->get_TopRightPoint()->get_Y();
			h_cp_z[objectPointIndex] = get_MyWakePanels()->at(ii).at(jj)->get_TopRightPoint()->get_Z();

			objectPointIndex++;

			// Top Left.
			h_cp_x[objectPointIndex] = get_MyWakePanels()->at(ii).at(jj)->get_TopLeftPoint()->get_X();
			h_cp_y[objectPointIndex] = get_MyWakePanels()->at(ii).at(jj)->get_TopLeftPoint()->get_Y();
			h_cp_z[objectPointIndex] = get_MyWakePanels()->at(ii).at(jj)->get_TopLeftPoint()->get_Z();

			objectPointIndex++;
		}
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Host object point coordinates initialized successfully. objectPointIndex: %i", objectPointIndex);

	// =========================================================================================
	// Now initialize the vortex segment end point data for all surface subject (bound) panels.
	int subjectSurfacePanelIndex = 0; // Panel index (counts over all the surface panels).

	for (int kk = 0; kk < project->get_MySurfaces()->size(); kk++) // DOM: Loop through all the surfaces.
	{
		for (int ii = 0; ii < project->get_MySurfaces()->at(kk)->get_MyPanels()->size(); ii++) // DOM: Loop through the rows of panels on each surface.
		{
			for (int jj = 0; jj < project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).size(); jj++) // DOM: Loop through the columns of panels on each surface.
			{
				// DOM: Vortex data.
				// DOM: Loop through all the bound vortices on the current panel.
				for (int mm = 0; mm < project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->size(); mm++)
				{
					// DOM: Vortex coordinates.
					h_vs_x[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = 
						project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_X();
					h_vs_y[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = 
						project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_Y();
					h_vs_z[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = 
						project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_Z();

					h_ve_x[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = 
						project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_X();
					h_ve_y[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = 
						project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_Y();
					h_ve_z[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = 
						project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_Z();
				}

				// DOM: Host vorticity assignments
				h_vorticities[subjectSurfacePanelIndex] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVorticity();

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "panel vorticity: %f, Vorticity: %f", 
					project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVorticity(), h_vorticities[subjectSurfacePanelIndex]);

				// Increment panel index.
				subjectSurfacePanelIndex++;
			}
		}
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Host surface subject panel vortex data initialized successfully. subjectSurfacePanelIndex: %i", subjectSurfacePanelIndex);

	// =========================================================================================
	// Now initialize the vortex segment end point data for (free) wake subject panels.
	int subjectWakePanelIndex = subjectSurfacePanelIndex; // Panel index (counts over all the wake panels).

	for (int kk = 0; kk < project->get_MySurfaces()->size(); kk++) // DOM: Loop through all the surfaces.
	{
		for (int ii = firstGeneration; ii < lastGeneration; ii++) // DOM: Loop through the generations of wake panels of the current surface.
		{
			for (int jj = 0; jj < project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).size(); jj++) // DOM: Loop through the columns of panels on each surface.
			{
				// DOM: Vortex data.
				// DOM: Loop through all the bound vortices on the current panel.
				for (int mm = 0; mm < project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->size(); mm++)
				{
					// DOM: Vortex coordinates.
					h_vs_x[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = 
						project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_X();
					h_vs_y[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = 
						project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_Y();
					h_vs_z[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = 
						project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_Z();

					h_ve_x[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = 
						project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_X();
					h_ve_y[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = 
						project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_Y();
					h_ve_z[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = 
						project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_Z();
				}

				// DOM: Host vorticity assignments
				h_vorticities[subjectWakePanelIndex] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVorticity();
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Host wake vorticity: %f", h_vorticities[subjectWakePanelIndex]);

				// Increment panel index.
				subjectWakePanelIndex++;
			}
		}
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Host wake subject panel vortex data initialized successfully. subjectWakePanelIndex: %i", subjectWakePanelIndex);

	// =========================================================================================
	// DOM: Copy data from host to device, do Cuda calculations, and copy data back to host. (CudaPhysics.cu)
	LARGE_INTEGER t1, t2, frequency;
	double elapsedTime;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);

	// Do the calculation in CudaPhysics.cu.
	ComputeVelocitiesForBatchOfPointsCuda(
		h_cp_x,
		h_cp_y,
		h_cp_z,
		h_vs_x,
		h_vs_y,
		h_vs_z,
		h_ve_x,
		h_ve_y,
		h_ve_z,
		h_cp_vx,
		h_cp_vy,
		h_cp_vz,
		h_vorticities,
		noOfVorticesPerPanel,
		noOfSubjectPanels,
		noOfVelocityPredictions,
		project->get_RankineAlgorithm().at(0));

	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "=========================== Inside ITSurface propagateWakeGeometryCuda. CUDA Batch time: %f msecs", elapsedTime);

	// =========================================================================================
	// Fill temp arrays with temp ITPoints representing the new coordinates of the wake panel vertices.
	int arrayIndex = 0;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);

	for (int i = firstGeneration; i < lastGeneration; i++) // Loop over generations.
	{
		std::vector <ITPoint*> *dummyBottomLeft = new std::vector <ITPoint*>;
		std::vector <ITPoint*> *dummyBottomRight = new std::vector <ITPoint*>;
		std::vector <ITPoint*> *dummyTopRight = new std::vector <ITPoint*>;
		std::vector <ITPoint*> *dummyTopLeft = new std::vector <ITPoint*>;

		for (int j = 0; j < get_MyWakePanels()->at(i).size(); j++)
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "vx: %f, vy: %f, vz: %f", h_cp_vx[arrayIndex], h_cp_vy[arrayIndex], h_cp_vz[arrayIndex]);

			ITWakePanel *p = get_MyWakePanels()->at(i).at(j);

			ITPoint *blPt = new ITPoint(0.0f, 0.0f, 0.0f);
			propagateWakePanelPointBasedOnCudaResults(blPt, p->get_BottomLeftPoint(), h_cp_vx[arrayIndex], h_cp_vy[arrayIndex], h_cp_vz[arrayIndex], DELTA_T);
			dummyBottomLeft->push_back(blPt);
			arrayIndex++;

			ITPoint *brPt = new ITPoint(0.0f, 0.0f, 0.0f);
			propagateWakePanelPointBasedOnCudaResults(brPt, p->get_BottomRightPoint(), h_cp_vx[arrayIndex], h_cp_vy[arrayIndex], h_cp_vz[arrayIndex], DELTA_T);
			dummyBottomRight->push_back(brPt);
			arrayIndex++;

			ITPoint *trPt = new ITPoint(0.0f, 0.0f, 0.0f);
			propagateWakePanelPointBasedOnCudaResults(trPt, p->get_TopRightPoint(), h_cp_vx[arrayIndex], h_cp_vy[arrayIndex], h_cp_vz[arrayIndex], DELTA_T);
			dummyTopRight->push_back(trPt);
			arrayIndex++;

			ITPoint *tlPt = new ITPoint(0.0f, 0.0f, 0.0f);
			propagateWakePanelPointBasedOnCudaResults(tlPt, p->get_TopLeftPoint(), h_cp_vx[arrayIndex], h_cp_vy[arrayIndex], h_cp_vz[arrayIndex], DELTA_T);
			dummyTopLeft->push_back(tlPt);
			arrayIndex++;
		}

		tempBottomLeft->push_back(*dummyBottomLeft);
		tempBottomRight->push_back(*dummyBottomRight);
		tempTopRight->push_back(*dummyTopRight);
		tempTopLeft->push_back(*dummyTopLeft);
	}

	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, 
		"=========================== Inside ITSurface propagateWakeGeometryCuda. propagateWakePanelPointBasedOnCudaResults time: %f msecs", elapsedTime);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Array Index: %i, noOfVelocityPredictions: %i", arrayIndex, noOfVelocityPredictions);

	// =========================================================================================
	// Free host memory.
	free(h_cp_x);
	free(h_cp_y);
	free(h_cp_z);

	free(h_vs_x);
	free(h_vs_y);
	free(h_vs_z);

	free(h_ve_x);
	free(h_ve_y);
	free(h_ve_z);

	free(h_cp_vx);
	free(h_cp_vy);
	free(h_cp_vz);

	free(h_vorticities);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Host arrays freed successfully.");

	// ==================================================================================================
	// 2. Re-assign the actual vortex end point coordinates.
	// ==================================================================================================
	for (int i = firstGeneration; i < lastGeneration; i++) // Loop over the generations.
	{
		int adjustedI = i - firstGeneration;

		for (int j = 0; j < get_MyWakePanels()->at(i).size(); j++) // Loop down the trailing edge panels.
		{
			ITWakePanel *p = get_MyWakePanels()->at(i).at(j);

			// Before updating vortices, compute existing perimeter length.
			float old_perimeter_length = 0.0f;
			for (int t = 0; t < p->get_MyVortices()->size(); t++)
			{
				old_perimeter_length = old_perimeter_length + p->get_MyVortices()->at(t)->get_MyLength();
			}

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Old perimeter length: %f ", old_perimeter_length);

			// Update the vortex geometry.
			// Vortices are in the following order: bottom, right, top, left (in anti-clockwise direction)
			// Bottom.
			ITVortex *v0 = p->get_MyVortices()->at(0);
			v0->get_StartPoint()->set_VX(tempBottomLeft->at(adjustedI).at(j)->get_VX());
			v0->get_StartPoint()->set_VY(tempBottomLeft->at(adjustedI).at(j)->get_VY());
			v0->get_StartPoint()->set_VZ(tempBottomLeft->at(adjustedI).at(j)->get_VZ());
			v0->get_StartPoint()->set_X(tempBottomLeft->at(adjustedI).at(j)->get_X());
			v0->get_StartPoint()->set_Y(tempBottomLeft->at(adjustedI).at(j)->get_Y());
			v0->get_StartPoint()->set_Z(tempBottomLeft->at(adjustedI).at(j)->get_Z());

			v0->get_EndPoint()->set_VX(tempBottomRight->at(adjustedI).at(j)->get_VX());
			v0->get_EndPoint()->set_VY(tempBottomRight->at(adjustedI).at(j)->get_VY());
			v0->get_EndPoint()->set_VZ(tempBottomRight->at(adjustedI).at(j)->get_VZ());
			v0->get_EndPoint()->set_X(tempBottomRight->at(adjustedI).at(j)->get_X());
			v0->get_EndPoint()->set_Y(tempBottomRight->at(adjustedI).at(j)->get_Y());
			v0->get_EndPoint()->set_Z(tempBottomRight->at(adjustedI).at(j)->get_Z());

			v0->calculateMyLength();

			// Right.
			ITVortex *v1 = p->get_MyVortices()->at(1); // Reuse the pointer v.
			v1->get_StartPoint()->set_VX(tempBottomRight->at(adjustedI).at(j)->get_VX());
			v1->get_StartPoint()->set_VY(tempBottomRight->at(adjustedI).at(j)->get_VY());
			v1->get_StartPoint()->set_VZ(tempBottomRight->at(adjustedI).at(j)->get_VZ());
			v1->get_StartPoint()->set_X(tempBottomRight->at(adjustedI).at(j)->get_X());
			v1->get_StartPoint()->set_Y(tempBottomRight->at(adjustedI).at(j)->get_Y());
			v1->get_StartPoint()->set_Z(tempBottomRight->at(adjustedI).at(j)->get_Z());

			v1->get_EndPoint()->set_VX(tempTopRight->at(adjustedI).at(j)->get_VX());
			v1->get_EndPoint()->set_VY(tempTopRight->at(adjustedI).at(j)->get_VY());
			v1->get_EndPoint()->set_VZ(tempTopRight->at(adjustedI).at(j)->get_VZ());
			v1->get_EndPoint()->set_X(tempTopRight->at(adjustedI).at(j)->get_X());
			v1->get_EndPoint()->set_Y(tempTopRight->at(adjustedI).at(j)->get_Y());
			v1->get_EndPoint()->set_Z(tempTopRight->at(adjustedI).at(j)->get_Z());

			v1->calculateMyLength();

			// Top.
			ITVortex *v2 = p->get_MyVortices()->at(2);
			v2->get_StartPoint()->set_VX(tempTopRight->at(adjustedI).at(j)->get_VX());
			v2->get_StartPoint()->set_VY(tempTopRight->at(adjustedI).at(j)->get_VY());
			v2->get_StartPoint()->set_VZ(tempTopRight->at(adjustedI).at(j)->get_VZ());
			v2->get_StartPoint()->set_X(tempTopRight->at(adjustedI).at(j)->get_X());
			v2->get_StartPoint()->set_Y(tempTopRight->at(adjustedI).at(j)->get_Y());
			v2->get_StartPoint()->set_Z(tempTopRight->at(adjustedI).at(j)->get_Z());

			v2->get_EndPoint()->set_VX(tempTopLeft->at(adjustedI).at(j)->get_VX());
			v2->get_EndPoint()->set_VY(tempTopLeft->at(adjustedI).at(j)->get_VY());
			v2->get_EndPoint()->set_VZ(tempTopLeft->at(adjustedI).at(j)->get_VZ());
			v2->get_EndPoint()->set_X(tempTopLeft->at(adjustedI).at(j)->get_X());
			v2->get_EndPoint()->set_Y(tempTopLeft->at(adjustedI).at(j)->get_Y());
			v2->get_EndPoint()->set_Z(tempTopLeft->at(adjustedI).at(j)->get_Z());

			v2->calculateMyLength();

			// Left.
			ITVortex *v3 = p->get_MyVortices()->at(3);
			v3->get_StartPoint()->set_VX(tempTopLeft->at(adjustedI).at(j)->get_VX());
			v3->get_StartPoint()->set_VY(tempTopLeft->at(adjustedI).at(j)->get_VY());
			v3->get_StartPoint()->set_VZ(tempTopLeft->at(adjustedI).at(j)->get_VZ());
			v3->get_StartPoint()->set_X(tempTopLeft->at(adjustedI).at(j)->get_X());
			v3->get_StartPoint()->set_Y(tempTopLeft->at(adjustedI).at(j)->get_Y());
			v3->get_StartPoint()->set_Z(tempTopLeft->at(adjustedI).at(j)->get_Z());

			v3->get_EndPoint()->set_VX(tempBottomLeft->at(adjustedI).at(j)->get_VX());
			v3->get_EndPoint()->set_VY(tempBottomLeft->at(adjustedI).at(j)->get_VY());
			v3->get_EndPoint()->set_VZ(tempBottomLeft->at(adjustedI).at(j)->get_VZ());
			v3->get_EndPoint()->set_X(tempBottomLeft->at(adjustedI).at(j)->get_X());
			v3->get_EndPoint()->set_Y(tempBottomLeft->at(adjustedI).at(j)->get_Y());
			v3->get_EndPoint()->set_Z(tempBottomLeft->at(adjustedI).at(j)->get_Z());

			v3->calculateMyLength();

			// ==================================================================================================
			// 3. Re-assign ITPanel vertex coordinates and velocities from temporary vectors of vectors.
			// ==================================================================================================

			// Bottom left
			p->get_BottomLeftPoint()->set_VX(tempBottomLeft->at(adjustedI).at(j)->get_VX());
			p->get_BottomLeftPoint()->set_VY(tempBottomLeft->at(adjustedI).at(j)->get_VY());
			p->get_BottomLeftPoint()->set_VZ(tempBottomLeft->at(adjustedI).at(j)->get_VZ());
			p->get_BottomLeftPoint()->set_X(tempBottomLeft->at(adjustedI).at(j)->get_X());
			p->get_BottomLeftPoint()->set_Y(tempBottomLeft->at(adjustedI).at(j)->get_Y());
			p->get_BottomLeftPoint()->set_Z(tempBottomLeft->at(adjustedI).at(j)->get_Z());

			// Bottom right
			p->get_BottomRightPoint()->set_VX(tempBottomRight->at(adjustedI).at(j)->get_VX());
			p->get_BottomRightPoint()->set_VY(tempBottomRight->at(adjustedI).at(j)->get_VY());
			p->get_BottomRightPoint()->set_VZ(tempBottomRight->at(adjustedI).at(j)->get_VZ());
			p->get_BottomRightPoint()->set_X(tempBottomRight->at(adjustedI).at(j)->get_X());
			p->get_BottomRightPoint()->set_Y(tempBottomRight->at(adjustedI).at(j)->get_Y());
			p->get_BottomRightPoint()->set_Z(tempBottomRight->at(adjustedI).at(j)->get_Z());

			// Top right
			p->get_TopRightPoint()->set_VX(tempTopRight->at(adjustedI).at(j)->get_VX());
			p->get_TopRightPoint()->set_VY(tempTopRight->at(adjustedI).at(j)->get_VY());
			p->get_TopRightPoint()->set_VZ(tempTopRight->at(adjustedI).at(j)->get_VZ());
			p->get_TopRightPoint()->set_X(tempTopRight->at(adjustedI).at(j)->get_X());
			p->get_TopRightPoint()->set_Y(tempTopRight->at(adjustedI).at(j)->get_Y());
			p->get_TopRightPoint()->set_Z(tempTopRight->at(adjustedI).at(j)->get_Z());

			// Top left
			p->get_TopLeftPoint()->set_VX(tempTopLeft->at(adjustedI).at(j)->get_VX());
			p->get_TopLeftPoint()->set_VY(tempTopLeft->at(adjustedI).at(j)->get_VY());
			p->get_TopLeftPoint()->set_VZ(tempTopLeft->at(adjustedI).at(j)->get_VZ());
			p->get_TopLeftPoint()->set_X(tempTopLeft->at(adjustedI).at(j)->get_X());
			p->get_TopLeftPoint()->set_Y(tempTopLeft->at(adjustedI).at(j)->get_Y());
			p->get_TopLeftPoint()->set_Z(tempTopLeft->at(adjustedI).at(j)->get_Z());

			// Update properties.
			p->computeDiagonals();
			p->computeMidPoint();
			p->computeColocationPoint();
			p->computeNormal();
			p->computeArea();

			// Recall that the vortex object end points are the ITPoint objects 
			// that are the parent ITPanel vertex ITPoints.
		}
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "About to free memory");

	// ==================================================================================================
	// 4. Free memory.
	// ==================================================================================================
	int m = tempBottomLeft->size();
	for (int i = 0; i < m; i++)
	{
		int n = tempBottomLeft->at(0).size();

		for (int j = 0; j < n; j++)
		{
			// Delete the point objects.
			delete tempBottomLeft->at(0).at(0);
			delete tempBottomRight->at(0).at(0);
			delete tempTopRight->at(0).at(0);
			delete tempTopLeft->at(0).at(0);

			// Remove the pointers from the vectors of vectors.
			tempBottomLeft->at(0).erase(tempBottomLeft->at(0).begin());
			tempBottomRight->at(0).erase(tempBottomRight->at(0).begin());
			tempTopRight->at(0).erase(tempTopRight->at(0).begin());
			tempTopLeft->at(0).erase(tempTopLeft->at(0).begin());
		}

		//Remove the pointers from the vectors.
		tempBottomLeft->erase(tempBottomLeft->begin());
		tempBottomRight->erase(tempBottomRight->begin());
		tempTopRight->erase(tempTopRight->begin());
		tempTopLeft->erase(tempTopLeft->begin());
	}

	tempBottomLeft->clear();
	tempBottomRight->clear();
	tempTopRight->clear();
	tempTopLeft->clear();

	delete tempBottomLeft;
	delete tempBottomRight;
	delete tempTopLeft;
	delete tempTopRight;

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "End of propagateWakeGeometryCuda");
}

void ITSurface::propagateWakePanelPointBasedOnCudaResults(ITPoint *tempPt, ITPoint *p, float vx, float vy, float vz, float DELTA_T)
{
	// Input parameters:
	//
	// tempPt - This ITPoint object was instanciated in the calling context, and the velocity and position of this point are initialized here.
	// p - This is the actual panel vertex point. This object is not updated.

	// Move temp point.
	// Euler integration.

	float gustVx = 0.0;
	float gustVy = 0.0;
	float gustVz = 0.0;

	if (project->get_IsGust())
	{
		float x = p->get_X();
		float y = p->get_Y();
		float z = p->get_Z();

		project->get_MyGust()->computeVelocityAtPoint(x, y, z, &gustVx, &gustVy, &gustVz);
	}

	tempPt->set_X(p->get_X() + DELTA_T*vx + DELTA_T*gustVx);
	tempPt->set_Y(p->get_Y() + DELTA_T*vy + DELTA_T*gustVy);
	tempPt->set_Z(p->get_Z() + DELTA_T*vz + DELTA_T*gustVz);

	// Store new velocity.
	tempPt->set_VX(vx);
	tempPt->set_VY(vy);
	tempPt->set_VZ(vz);
}

void ITSurface::moveMeBackToBase(int k)
{
	// This method sets the working surface geometry back to the Base surface geometry (undoing any morphing).
	for (int i = 0; i < get_MyControlPoints()->size(); i++)
	{
		for (int j = 0; j < get_MyControlPoints()->at(i).size(); j++)
		{
			ITPoint* currentBasePoint = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

			get_MyControlPoints()->at(i).at(j)->set_X(currentBasePoint->get_X());
			get_MyControlPoints()->at(i).at(j)->set_Y(currentBasePoint->get_Y());
			get_MyControlPoints()->at(i).at(j)->set_Z(currentBasePoint->get_Z());

		}
	}
}

std::vector < std::vector <ITControlPoint*> > *ITSurface::get_MyControlPoints() { return _MyControlPoints; }
void ITSurface::set_MyControlPoints(std::vector < std::vector <ITControlPoint*> > *myControlPoints) { _MyControlPoints = myControlPoints; }

std::vector <ITTrajectoryCurve*> *ITSurface::get_MyTrajectoryCurves() { return _MyTrajectoryCurves; }
void ITSurface::set_MyTrajectoryCurves(std::vector <ITTrajectoryCurve*> *myTrajectoryCurves) { _MyTrajectoryCurves = myTrajectoryCurves; }

std::vector <int> *ITSurface::get_MyAncestorSurfaceIndices() { return _MyAncestorSurfaceIndices; }
void ITSurface::set_MyAncestorSurfaceIndices(std::vector <int> *myAncestorSurfaceIndices) { _MyAncestorSurfaceIndices = myAncestorSurfaceIndices; }

std::vector < std::vector <ITPoint*> > *ITSurface::get_MyInterpolatedPoints() { return _MyInterpolatedPoints; }
void ITSurface::set_MyInterpolatedPoints(std::vector < std::vector <ITPoint*> > *myInterpolatedPoints) { _MyInterpolatedPoints = myInterpolatedPoints; }

std::vector < std::vector <ITPoint*> > *ITSurface::get_MyInterpolatedNormals() { return _MyInterpolatedNormals; }
void ITSurface::set_MyInterpolatedNormals(std::vector < std::vector <ITPoint*> > *myInterpolatedNormals) { _MyInterpolatedNormals = myInterpolatedNormals; }

std::vector < std::vector <ITPanel*> > *ITSurface::get_MyPanels() { return _MyPanels; }
void ITSurface::set_MyPanels(std::vector < std::vector <ITPanel*> > *myPanels) { _MyPanels = myPanels; }

std::vector < std::vector <ITWakePanel*> > *ITSurface::get_MyWakePanels() { return _MyWakePanels; }
void ITSurface::set_MyWakePanels(std::vector < std::vector <ITWakePanel*> > *myWakePanels) { _MyWakePanels = myWakePanels; }

std::vector <ITControlSurface*> *ITSurface::get_MyControlSurfaces() { return _MyControlSurfaces; }
void ITSurface::set_MyControlSurfaces(std::vector <ITControlSurface*> *a) { _MyControlSurfaces = a; }

std::vector < std::vector < std::vector <ITWakePanel*> > > *ITSurface::get_MyWakePanelHistory() { return _MyWakePanelHistory; }
void ITSurface::set_MyWakePanelHistory(std::vector < std::vector < std::vector <ITWakePanel*> > > *myWakePanelHistory) { _MyWakePanelHistory = myWakePanelHistory; }

std::vector < std::vector < std::vector <ITPoint*> > > *ITSurface::get_MyControlPointDeflectionHistory() { return _MyControlPointDeflectionHistory; }
void ITSurface::set_MyControlPointDeflectionHistory(std::vector < std::vector < std::vector <ITPoint*> > > 
	*myControlPointDeflectionHistory) { _MyControlPointDeflectionHistory = myControlPointDeflectionHistory; }

std::vector < std::vector < std::vector <float> > > *ITSurface::get_MyPressureHistory() { return _MyPressureHistory; }
void ITSurface::set_MyPressureHistory(std::vector < std::vector < std::vector <float> > > *myPressureHistory) { _MyPressureHistory = myPressureHistory; }

std::vector < std::vector < std::vector <float> > > *ITSurface::get_MyVorticityHistory() { return _MyVorticityHistory; }
void ITSurface::set_MyVorticityHistory(std::vector < std::vector < std::vector <float> > > *myVorticityHistory) { _MyVorticityHistory = myVorticityHistory; }

std::vector <float> *ITSurface::get_MyCLHistory() { return _MyCLHistory; }
void ITSurface::set_MyCLHistory(std::vector <float> *myCLHistory) { _MyCLHistory = myCLHistory; }

std::vector <float> *ITSurface::get_MyCDHistory() { return _MyCDHistory; }
void ITSurface::set_MyCDHistory(std::vector <float> *myCDHistory) { _MyCDHistory = myCDHistory; }

std::vector <ITPoint*> *ITSurface::get_MyForceHistory() { return _MyForceHistory; }
void ITSurface::set_MyForceHistory(std::vector <ITPoint*> *f) { _MyForceHistory = f; }

int ITSurface::get_NoOfInterpolatedPointsU() { return _NoOfInterpolatedPointsU; }
void ITSurface::set_NoOfInterpolatedPointsU(int u) { _NoOfInterpolatedPointsU = u; }
int ITSurface::get_NoOfInterpolatedPointsV() { return _NoOfInterpolatedPointsV; }
void ITSurface::set_NoOfInterpolatedPointsV(int v) { _NoOfInterpolatedPointsV = v; }

bool ITSurface::get_IsMorph() { return _IsMorph; }
void ITSurface::set_IsMorph(bool b) { _IsMorph = b; }
int ITSurface::get_MorphStartFrame() { return _MorphStartFrame; }
void ITSurface::set_MorphStartFrame(int f) { _MorphStartFrame = f; }
int ITSurface::get_MorphEndFrame() { return _MorphEndFrame; }
void ITSurface::set_MorphEndFrame(int f) { _MorphEndFrame = f; }
std::string ITSurface::get_MorphType() { return _MorphType; }
void ITSurface::set_MorphType(std::string s) { _MorphType = s; }

bool ITSurface::get_IsWake() { return _IsWake; }
void ITSurface::set_IsWake(bool b) { _IsWake = b; }
int ITSurface::get_ParentSurfaceIndex() { return _ParentSurfaceIndex; }
void ITSurface::set_ParentSurfaceIndex(int f) { _ParentSurfaceIndex = f; }
bool ITSurface::get_IsPistolesiPanelOffset() { return _IsPistolesiPanelOffset; }
void ITSurface::set_IsPistolesiPanelOffset(bool b) { _IsPistolesiPanelOffset = b; }
float ITSurface::get_PistolesiPanelOffsetFraction() { return _PistolesiPanelOffsetFraction; }
void ITSurface::set_PistolesiPanelOffsetFraction(float p) { _PistolesiPanelOffsetFraction = p; }
float ITSurface::get_FuselageRadius() { return _FuselageRadius; }
void ITSurface::set_FuselageRadius(float f) { _FuselageRadius = f; }

ITPoint *ITSurface::get_MyCentreOfRotationPoint() { return _MyCentreOfRotationPoint; }
void ITSurface::set_MyCentreOfRotationPoint(ITPoint* p) { _MyCentreOfRotationPoint = p; }

Eigen::MatrixXd ITSurface::get_MU() { return _MU; }
void ITSurface::set_MU(Eigen::MatrixXd mu) { _MU = mu; }

Eigen::MatrixXd ITSurface::get_MV() { return _MV; }
void ITSurface::set_MV(Eigen::MatrixXd mv) { _MV = mv; }

std::vector <float> *ITSurface::get_URange() { return _URange; }
void ITSurface::set_URange(std::vector <float> *ur) { _URange = ur; }

std::vector <float> *ITSurface::get_VRange() { return _VRange; }
void ITSurface::set_VRange(std::vector <float> *vr) { _VRange = vr; }

Eigen::MatrixXd ITSurface::get_UPowerMatrix() { return _UPowerMatrix; }
void ITSurface::set_UPowerMatrix(Eigen::MatrixXd upm) { _UPowerMatrix = upm; }

Eigen::MatrixXd ITSurface::get_VPowerMatrix() { return _VPowerMatrix; }
void ITSurface::set_VPowerMatrix(Eigen::MatrixXd vpm) { _VPowerMatrix = vpm; }

ITPoint *ITSurface::get_MyCurrentTranslationPoint() { return _MyCurrentTranslationPoint; }
void ITSurface::set_MyCurrentTranslationPoint(ITPoint* p) { _MyCurrentTranslationPoint = p; }

ITPoint *ITSurface::get_MyCurrentRotationPoint() { return _MyCurrentRotationPoint; }
void ITSurface::set_MyCurrentRotationPoint(ITPoint* p) { _MyCurrentRotationPoint = p; }

ITPoint *ITSurface::get_MyPreviousTranslationPoint() { return _MyPreviousTranslationPoint; }
void ITSurface::set_MyPreviousTranslationPoint(ITPoint* p) { _MyPreviousTranslationPoint = p; }

ITPoint *ITSurface::get_MyPreviousRotationPoint() { return _MyPreviousRotationPoint; }
void ITSurface::set_MyPreviousRotationPoint(ITPoint* p) { _MyPreviousRotationPoint = p; }

ITBeam *ITSurface::get_MyBeam() { return _MyBeam; }
void ITSurface::set_MyBeam(ITBeam* b) { _MyBeam = b; }