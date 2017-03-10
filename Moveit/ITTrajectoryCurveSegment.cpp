#include "ITTrajectoryCurveSegment.h"
#include "ITPointTrajectory.h"
#include "global.h"
#include "ITProject.h"
#include "ITPointTrajectory.h"
#include "ITPoint.h" // So that deletion of _m0_p and _m1_p work correctly.

ITTrajectoryCurveSegment::ITTrajectoryCurveSegment(void)
{
	set_Tension(0.5);
	set_NoOfStrides(60);
	set_ArcLength(0.0);

	_ArcLengthIncrements = new std::vector <float>;
	_ArcLengthCummulates = new std::vector <float>;
}

ITTrajectoryCurveSegment::~ITTrajectoryCurveSegment(void)
{
	// We should be deleting _P0_p and _P1_p and _m0_p and _m1_p here.
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside destructor. ITTrajectoryCurveSegment being destroyed.");

	delete _P0_p;
	delete _P1_p;
	delete _m0_p;
	delete _m1_p;

	// Delete the arc-length increments.
	get_ArcLengthIncrements()->clear();
	delete _ArcLengthIncrements;

	// Delete the arc-length cummulates.
	get_ArcLengthCummulates()->clear();
	delete _ArcLengthCummulates;
}

// Worker methods.
ITPoint *ITTrajectoryCurveSegment::evaluatePointAtParameterValue(float s)
{
	// Compute the basis functions.
	float basisP0 = 2.0f*s*s*s - 3.0f*s*s + 1.0f;
	float basisP1 = 3.0f*s*s - 2.0f*s*s*s;
	float basism0 = s - 2.0f*s*s + s*s*s;
	float basism1 = s*s*s - s*s;

	// Compute the coordinates.
	float x = get_StartKeyFrame()*basisP0 + get_EndKeyFrame()*basisP1 + get_m0_p()->get_X()*basism0 + get_m1_p()->get_X()*basism1;
	float y = get_P0_p()->get_X()*basisP0 + get_P1_p()->get_X()*basisP1 + get_m0_p()->get_Y()*basism0 + get_m1_p()->get_Y()*basism1;
	float z = 0.0;

	// Instanciate and return the new ITPoint object.
	// The x instance variable of the ITPoint object contains the fractional frame number - indexed from the start of the segment.
	// The y instance variable of the ITPoint object contains the parameter value.
	ITPoint *p = new ITPoint(x, y, z);
	return p;
}

// This member function computes the arc-length of this segment for a set of _NoOfStrides+1 equally spaced parameter values.
// The result is used to construct an inverse function (in the member function calculateParameterForDistance) to 
// compute a parameter value for a given distance along the curve.
void ITTrajectoryCurveSegment::calculateData()
{
	// Empty arc length vectors.
	if (!get_ArcLengthIncrements()->empty())
	{
		get_ArcLengthIncrements()->clear();
	}
	if (!get_ArcLengthCummulates()->empty())
	{
		get_ArcLengthCummulates()->clear();
	}

	// Loop over the interpolation points. The loop is actually executed NoOfStrides+1 times.
	float s = 0.0f; // The parameter value runs over the interval from 0 to 1.

	float deltaT = (float)(1 / (float)get_NoOfStrides());

	float il = 0.0f; // incrementing arc-length of this segment.
	float lastValue = 0.0;

	for (unsigned long int j = 0; j < get_NoOfStrides(); j++)
	{
		// Find the point on segment corresponding to current value of s.
		ITPoint *p = evaluatePointAtParameterValue(s);

		// Accumulate the arc-length of this segment.
		il = p->get_Y() - lastValue;

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "============== %f %f", s, p->get_Y());

		// Put incrementted arc-length of this segment in the instance variable vector.
		get_ArcLengthIncrements()->push_back(il);

		// Put cummulated arc-length of this segment in the instance variable vector.
		get_ArcLengthCummulates()->push_back(p->get_Y());

		// Save last value.
		lastValue = p->get_Y();

		delete p;

		// Increment the parameter s.
		s = s + deltaT;
	}

	// Store the total arc-length of this segment in an instance variable.
	set_ArcLength(lastValue);
}

float ITTrajectoryCurveSegment::calculateParameterForDistance(float D)
{
	// Loop around the segment length cummulate vector until we find an
	// cummulated length just larger than the distance D.
	int j;

	//if (get_P0_p()->get_X() <= get_P1_p()->get_X()) // Step through until arc length greater than D
	//{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "============== Counting up.");
	for (j = 0; j < (int)((*get_ArcLengthCummulates()).size()); j++)
	{
		if ((fabs((double)(*get_ArcLengthCummulates()).at(j))) > D)
		{
			break;
		}
	}
	//}
	//else // 
	//{
	//	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "============== Counting down.");
	//	for (j=0; j<(int)((*get_ArcLengthCummulates()).size()); j++)
	//	{

	//		if ( (fabs( (double)(*get_ArcLengthCummulates()).at(j))  ) > D )
	//		{
	//			break;
	//		}
	//	}

	//}
	// Look up the value of s corresponding to the j-th entry in the incremental length vector.
	float s = (float)(j) / (float)(get_NoOfStrides());

	return s;
}

// Accessors.
ITPointTrajectory *ITTrajectoryCurveSegment::get_P0_p() { return _P0_p; }
void ITTrajectoryCurveSegment::set_P0_p(ITPointTrajectory *p) { _P0_p = p; }

ITPointTrajectory *ITTrajectoryCurveSegment::get_P1_p() { return _P1_p; }
void ITTrajectoryCurveSegment::set_P1_p(ITPointTrajectory *p) { _P1_p = p; }

ITPoint *ITTrajectoryCurveSegment::get_m0_p() { return _m0_p; }
void ITTrajectoryCurveSegment::set_m0_p(ITPoint *m) { _m0_p = m; }

ITPoint *ITTrajectoryCurveSegment::get_m1_p() { return _m1_p; }
void ITTrajectoryCurveSegment::set_m1_p(ITPoint *m) { _m1_p = m; }

float ITTrajectoryCurveSegment::get_Tension() { return _Tension; }
void ITTrajectoryCurveSegment::set_Tension(float a) { _Tension = a; }

int ITTrajectoryCurveSegment::get_StartKeyFrame() { return _StartKeyFrame; }
void ITTrajectoryCurveSegment::set_StartKeyFrame(int a) { _StartKeyFrame = a; }

int ITTrajectoryCurveSegment::get_EndKeyFrame() { return _EndKeyFrame; }
void ITTrajectoryCurveSegment::set_EndKeyFrame(int a) { _EndKeyFrame = a; }

int ITTrajectoryCurveSegment::get_NoOfStrides() { return _NoOfStrides; }
void ITTrajectoryCurveSegment::set_NoOfStrides(int n) { _NoOfStrides = n; }

std::vector <float> *ITTrajectoryCurveSegment::get_ArcLengthIncrements() { return _ArcLengthIncrements; }
void ITTrajectoryCurveSegment::set_ArcLengthIncrements(std::vector <float> *v) { _ArcLengthIncrements = v; }

std::vector <float> *ITTrajectoryCurveSegment::get_ArcLengthCummulates() { return _ArcLengthCummulates; }
void ITTrajectoryCurveSegment::set_ArcLengthCummulates(std::vector <float> *v) { _ArcLengthCummulates = v; }

float ITTrajectoryCurveSegment::get_ArcLength() { return _ArcLength; }
void ITTrajectoryCurveSegment::set_ArcLength(float l) { _ArcLength = l; }