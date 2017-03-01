// Dom's includes.
#include "global.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITControlPoint.h"
#include "ITPointTrajectory.h"
#include "ITTrajectoryCurve.h"
#include "ITTrajectoryCurveSegment.h"

ITSurface::ITSurface(void)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside constructor. ITSurface being created.");

	_MyControlPoints = new std::vector < std::vector <ITControlPoint*> >;
	_MyFocusControlPoints = new std::vector <ITControlPoint*>; // This contains pointers to control points. No new control points are created.

	_MyInterpolatedPoints = new std::vector < std::vector <ITPoint*> >;
	_MyInterpolatedNormals = new std::vector < std::vector <ITPoint*> >;

	_MyTrajectoryCurves = new std::vector <ITTrajectoryCurve *>;

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



	// Delte trajectory curves.
	int noOfTrajCurves = get_MyTrajectoryCurves()->size();
	for (int i=0; i<noOfTrajCurves; i++)
	{
		delete get_MyTrajectoryCurves()->at(i);
	}

	get_MyTrajectoryCurves()->clear();
	delete _MyTrajectoryCurves;


	// Delete the centre of rotation point.
	delete _MyCentreOfRotationPoint;

	delete _MyCurrentTranslationPoint;
	delete _MyCurrentRotationPoint;
	delete _MyPreviousTranslationPoint;
	delete _MyPreviousRotationPoint;

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
 
	computeTrajectoryPointsAtFrame(FrameNumber, k, translationPoint, rotationPoint);

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

		computeTrajectoryPointsAtFrame(FrameNumber-1, k, translationPoint, rotationPoint);
    }
	else
	{
		// We are at FrameNumber = 0 so initialize the previous points to the same as those for FrameNumber=1.
		// Setting the previous trajectory points identical to those for ther current frame number when FrameNumber = 0 will ensure that the initial panel velocities are zero.
		// This will reduce the violent starting vortex.
		computeTrajectoryPointsAtFrame(FrameNumber, k, translationPoint, rotationPoint);
	}        

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "translation x: %f, translation y: %f, translation z: %f. FrameNumber = %i", translationPoint->get_X(), translationPoint->get_Y(), translationPoint->get_Z(), FrameNumber);

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
    for (int i=0; i<get_MyControlPoints()->size(); i++)
    {
        for (int j=0; j<get_MyControlPoints()->at(i).size(); j++)
        {
            // Assign the coordinates of the current control point base position to the currentPoint temporary object.

			// Ignore deformation.
			currentPoint->set_X( project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X() );
			currentPoint->set_Y( project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y() );
			currentPoint->set_Z( project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z() );

			currentPoint->propagateMe(cp, get_MyCurrentRotationPoint(), get_MyCurrentTranslationPoint());

            // Re-assign control point coordinates.
            get_MyControlPoints()->at(i).at(j)->set_X( currentPoint->get_X() );
            get_MyControlPoints()->at(i).at(j)->set_Y( currentPoint->get_Y() );
            get_MyControlPoints()->at(i).at(j)->set_Z( currentPoint->get_Z() );
            
        }
    }

	// Delete the temporary object.
	delete currentPoint;

    // Now compute newly propagated interpolated points.
	manageComputationOfInterpolatedPoints();
        
    return;
    
} // End of propagateGeometry.



void ITSurface::moveMeBackToBase(int k)
{
    // This method sets the working surface geometry back to the Base surface geometry (undoing any morphing).
    for (int i=0; i<get_MyControlPoints()->size(); i++)
    {
        for (int j=0; j<get_MyControlPoints()->at(i).size(); j++)
        {
            ITPoint* currentBasePoint = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

            get_MyControlPoints()->at(i).at(j)->set_X( currentBasePoint->get_X() );
            get_MyControlPoints()->at(i).at(j)->set_Y( currentBasePoint->get_Y() );
            get_MyControlPoints()->at(i).at(j)->set_Z( currentBasePoint->get_Z() );
            
        }
    }  
} // End of moveMeBackToBase.





void ITSurface::computeTrajectoryPointsAtFrame(int myFrameNumber, int mySurfaceIndex, ITPoint* translationPoint, ITPoint* rotationPoint)
{
    
    project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside computeTrajectoryPointsAtFrame");

	// Find kStar.
    // Loop through the segments of the curve to find kStar, 
	// the index of the segment in which myFrameNumber lies.

	int kStar = 0;

	// Test if kStar is in fact smaller.
    for (int k=0; k<get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size(); k++)
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
    int noOfFramesInCurrentSegment = get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(kStar)->get_EndKeyFrame() - get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(kStar)->get_StartKeyFrame();
      
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
		float x = y0 + ((xx - x0)/(x1 - x0))*(y1 - y0);

		// y
		x0 = (float)(ySegment->get_StartKeyFrame());
		x1 = (float)(ySegment->get_EndKeyFrame());
		xx = (float)(myFrameNumber);
		y0 = ySegment->get_P0_p()->get_X();
		y1 = ySegment->get_P1_p()->get_X();
		float y = y0 + ((xx - x0)/(x1 - x0))*(y1 - y0);

		// z
		x0 = (float)(zSegment->get_StartKeyFrame());
		x1 = (float)(zSegment->get_EndKeyFrame());
		xx = (float)(myFrameNumber);
		y0 = zSegment->get_P0_p()->get_X();
		y1 = zSegment->get_P1_p()->get_X();
		float z = y0 + ((xx - x0)/(x1 - x0))*(y1 - y0);

		// r
		x0 = (float)(rSegment->get_StartKeyFrame());
		x1 = (float)(rSegment->get_EndKeyFrame());
		xx = (float)(myFrameNumber);
		y0 = rSegment->get_P0_p()->get_X();
		y1 = rSegment->get_P1_p()->get_X();
		float r = y0 + ((xx - x0)/(x1 - x0))*(y1 - y0);

		// p
		x0 = (float)(pSegment->get_StartKeyFrame());
		x1 = (float)(pSegment->get_EndKeyFrame());
		xx = (float)(myFrameNumber);
		y0 = pSegment->get_P0_p()->get_X();
		y1 = pSegment->get_P1_p()->get_X();
		float p = y0 + ((xx - x0)/(x1 - x0))*(y1 - y0);

		// yaw
		x0 = (float)(yawSegment->get_StartKeyFrame());
		x1 = (float)(yawSegment->get_EndKeyFrame());
		xx = (float)(myFrameNumber);
		y0 = yawSegment->get_P0_p()->get_X();
		y1 = yawSegment->get_P1_p()->get_X();
		float yaw = y0 + ((xx - x0)/(x1 - x0))*(y1 - y0);

		// Put the data in translationPoint and ratationPoint
		translationPoint->set_X( x );
		translationPoint->set_Y( y );
		translationPoint->set_Z( z );

		rotationPoint->set_X( r );
		rotationPoint->set_Y( p );
		rotationPoint->set_Z( yaw );


	}
	else
	{
		Tx = (float)deltaFrames/(float)noOfFramesInCurrentSegment;
		Ty = (float)deltaFrames/(float)noOfFramesInCurrentSegment;
		Tz = (float)deltaFrames/(float)noOfFramesInCurrentSegment;
		Tr = (float)deltaFrames/(float)noOfFramesInCurrentSegment;
		Tp = (float)deltaFrames/(float)noOfFramesInCurrentSegment;
		Tyaw = (float)deltaFrames/(float)noOfFramesInCurrentSegment;

		ITPoint * px = xSegment->evaluatePointAtParameterValue(Tx);
		ITPoint * py = ySegment->evaluatePointAtParameterValue(Ty);
		ITPoint * pz = zSegment->evaluatePointAtParameterValue(Tz);
		ITPoint * pr = rSegment->evaluatePointAtParameterValue(Tr);
		ITPoint * pp = pSegment->evaluatePointAtParameterValue(Tp);
		ITPoint * pyaw = yawSegment->evaluatePointAtParameterValue(Tyaw);

		// Put the data in translationPoint and ratationPoint
		translationPoint->set_X( px->get_Y() );
		translationPoint->set_Y( py->get_Y() );
		translationPoint->set_Z( pz->get_Y() );

		rotationPoint->set_X( pr->get_Y() );
		rotationPoint->set_Y( pp->get_Y() );
		rotationPoint->set_Z( pyaw->get_Y() );

		delete px;
		delete py;
		delete pz;
		delete pr;
		delete pp;
		delete pyaw;

	}


	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Returning from computeTrajectoryPointsAtFrame");

} // End of computeTrajectoryPointsAtFrame.




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

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "rows: %i, cols: %i", rows, cols);

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

std::vector <ITTrajectoryCurve*> *ITSurface::get_MyTrajectoryCurves(){ return _MyTrajectoryCurves; }
void set_MyTrajectoryCurves( std::vector <ITTrajectoryCurve*> *myTrajectoryCurves );

ITPoint *ITSurface::get_MyCentreOfRotationPoint(){ return _MyCentreOfRotationPoint; }
void ITSurface::set_MyCentreOfRotationPoint(ITPoint* p){ _MyCentreOfRotationPoint = p; }


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

ITPoint *ITSurface::get_MyCurrentTranslationPoint(){ return _MyCurrentTranslationPoint; }
void ITSurface::set_MyCurrentTranslationPoint(ITPoint* p){ _MyCurrentTranslationPoint = p; }

ITPoint *ITSurface::get_MyCurrentRotationPoint(){ return _MyCurrentRotationPoint; }
void ITSurface::set_MyCurrentRotationPoint(ITPoint* p){ _MyCurrentRotationPoint = p; }

ITPoint *ITSurface::get_MyPreviousTranslationPoint(){ return _MyPreviousTranslationPoint; }
void ITSurface::set_MyPreviousTranslationPoint(ITPoint* p){ _MyPreviousTranslationPoint = p; }

ITPoint *ITSurface::get_MyPreviousRotationPoint(){ return _MyPreviousRotationPoint; }
void ITSurface::set_MyPreviousRotationPoint(ITPoint* p){ _MyPreviousRotationPoint = p; }

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
