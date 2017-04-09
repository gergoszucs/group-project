#include "global.h"
#include "ITProject.h"
#include "ITPoint.h"
#include "ITTrajectoryCurve.h"
#include "ITTrajectoryCurveSegment.h"
#include "ITPointTrajectory.h"

ITTrajectoryCurve::ITTrajectoryCurve(void)
{
	_MyTrajectoryCurveSegments = new std::vector <ITTrajectoryCurveSegment *>;
	_MyFocusTrajectoryCurveSegmentIndices = new std::vector <int>;
}

ITTrajectoryCurve::~ITTrajectoryCurve(void)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside destructor. ITTrajectoryCurve being destroyed.");

	// Delete the curve segments.
	int noOfSegs = get_MyTrajectoryCurveSegments()->size();
	for (int i = 0; i < noOfSegs; i++)
	{
		delete get_MyTrajectoryCurveSegments()->at(i);
	}

	get_MyTrajectoryCurveSegments()->clear();
	delete _MyTrajectoryCurveSegments;

	// Delete the focus curve segment vector. (Note that since this vector only contains pointers, we should not delete the objects).
	get_MyFocusTrajectoryCurveSegmentIndices()->clear();
	delete _MyFocusTrajectoryCurveSegmentIndices;
}

// Worker methods.
void ITTrajectoryCurve::computeMySegmentEndTangentVectors()
{
	// Note that all curve segment end-points have the ordinate in the x instance variable, and the abscissa (FrameNumber) in the frame number instance variable.
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside computeMySegmentEndTangentVectors");

	// Loop through my curve segments.
	for (int i = 0; i < get_MyTrajectoryCurveSegments()->size(); i++)
	{
		ITPoint *local_m0_p, *local_m1_p;

		if (i == 0)
		{
			// We're on the first segment.
			float dx0 = (float)(get_MyTrajectoryCurveSegments()->at(i)->get_EndKeyFrame() - get_MyTrajectoryCurveSegments()->at(i)->get_StartKeyFrame());
			float dy0 = get_MyTrajectoryCurveSegments()->at(i)->get_P1_p()->get_X() - get_MyTrajectoryCurveSegments()->at(i)->get_P0_p()->get_X();
			local_m0_p = new ITPoint(dx0*get_MyTrajectoryCurveSegments()->at(i)->get_Tension(), dy0*get_MyTrajectoryCurveSegments()->at(i)->get_Tension(), 0.0);

			float dx1 = (float)(get_MyTrajectoryCurveSegments()->at(i + 1)->get_EndKeyFrame() - get_MyTrajectoryCurveSegments()->at(i)->get_StartKeyFrame());
			float dy1 = get_MyTrajectoryCurveSegments()->at(i + 1)->get_P1_p()->get_X() - get_MyTrajectoryCurveSegments()->at(i)->get_P0_p()->get_X();
			local_m1_p = new ITPoint(dx1*get_MyTrajectoryCurveSegments()->at(i)->get_Tension(), dy1*get_MyTrajectoryCurveSegments()->at(i)->get_Tension(), 0.0);
		}
		else if (i == get_MyTrajectoryCurveSegments()->size() - 1)
		{
			// We're on the last segment.
			float dx0 = (float)(get_MyTrajectoryCurveSegments()->at(i)->get_EndKeyFrame() - get_MyTrajectoryCurveSegments()->at(i - 1)->get_StartKeyFrame());
			float dy0 = get_MyTrajectoryCurveSegments()->at(i)->get_P1_p()->get_X() - get_MyTrajectoryCurveSegments()->at(i - 1)->get_P0_p()->get_X();
			local_m0_p = new ITPoint(dx0*get_MyTrajectoryCurveSegments()->at(i)->get_Tension(), dy0*get_MyTrajectoryCurveSegments()->at(i)->get_Tension(), 0.0);

			float dx1 = (float)(get_MyTrajectoryCurveSegments()->at(i)->get_EndKeyFrame() - get_MyTrajectoryCurveSegments()->at(i)->get_StartKeyFrame());
			float dy1 = get_MyTrajectoryCurveSegments()->at(i)->get_P1_p()->get_X() - get_MyTrajectoryCurveSegments()->at(i)->get_P0_p()->get_X();
			local_m1_p = new ITPoint(dx1*get_MyTrajectoryCurveSegments()->at(i)->get_Tension(), dy1*get_MyTrajectoryCurveSegments()->at(i)->get_Tension(), 0.0);
		}
		else
		{
			// We're on an interior segment.
			float dx0 = (float)(get_MyTrajectoryCurveSegments()->at(i)->get_EndKeyFrame() - get_MyTrajectoryCurveSegments()->at(i - 1)->get_StartKeyFrame());
			float dy0 = get_MyTrajectoryCurveSegments()->at(i)->get_P1_p()->get_X() - get_MyTrajectoryCurveSegments()->at(i - 1)->get_P0_p()->get_X();
			local_m0_p = new ITPoint(dx0*get_MyTrajectoryCurveSegments()->at(i)->get_Tension(), dy0*get_MyTrajectoryCurveSegments()->at(i)->get_Tension(), 0.0);

			float dx1 = (float)(get_MyTrajectoryCurveSegments()->at(i + 1)->get_EndKeyFrame() - get_MyTrajectoryCurveSegments()->at(i)->get_StartKeyFrame());
			float dy1 = get_MyTrajectoryCurveSegments()->at(i + 1)->get_P1_p()->get_X() - get_MyTrajectoryCurveSegments()->at(i)->get_P0_p()->get_X();
			local_m1_p = new ITPoint(dx1*get_MyTrajectoryCurveSegments()->at(i)->get_Tension(), dy1*get_MyTrajectoryCurveSegments()->at(i)->get_Tension(), 0.0);
		}

		get_MyTrajectoryCurveSegments()->at(i)->set_m0_p(local_m0_p);
		get_MyTrajectoryCurveSegments()->at(i)->set_m1_p(local_m1_p);
	}
}

void ITTrajectoryCurve::addSegment(const float pointA_x, const float pointA_y, const float pointA_z, const int pointA_frame, const float pointA_U, const float pointA_V,
	const float pointB_x, const float pointB_y, const float pointB_z, const int pointB_frame, const float pointB_U, const float pointB_V)
{
	ITTrajectoryCurveSegment *s = new ITTrajectoryCurveSegment();

	// Set the p0 and p1 instance variables.
	ITPointTrajectory *p0rp = new ITPointTrajectory(pointA_x, pointA_y, pointA_z);
	ITPointTrajectory *p1rp = new ITPointTrajectory(pointB_x, pointB_y, pointB_z);

	s->set_P0_p(p0rp);
	s->set_P1_p(p1rp);
	s->set_StartKeyFrame(pointA_frame);
	s->set_EndKeyFrame(pointB_frame);

	s->get_P0_p()->set_U(pointA_U);
	s->get_P0_p()->set_V(pointA_V);
	s->get_P1_p()->set_U(pointB_U);
	s->get_P1_p()->set_V(pointB_V);

	get_MyTrajectoryCurveSegments()->push_back(s);
}
// Accessors.
std::vector <ITTrajectoryCurveSegment*> *ITTrajectoryCurve::get_MyTrajectoryCurveSegments() { return _MyTrajectoryCurveSegments; }
void ITTrajectoryCurve::set_MyTrajectoryCurveSegments(std::vector <ITTrajectoryCurveSegment*> *v) { _MyTrajectoryCurveSegments = v; }

std::vector <int> *ITTrajectoryCurve::get_MyFocusTrajectoryCurveSegmentIndices() { return _MyFocusTrajectoryCurveSegmentIndices; }
void ITTrajectoryCurve::set_MyFocusTrajectoryCurveSegmentIndices(std::vector <int> *v) { _MyFocusTrajectoryCurveSegmentIndices = v; }