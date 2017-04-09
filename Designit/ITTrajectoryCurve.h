#pragma once

#include <vector>

class ITTrajectoryCurveSegment;

class ITTrajectoryCurve
{
private:
	std::vector <ITTrajectoryCurveSegment*> *_MyTrajectoryCurveSegments;
	std::vector <int> *_MyFocusTrajectoryCurveSegmentIndices;

public:
	ITTrajectoryCurve(void);
	~ITTrajectoryCurve(void);

	// Worker methods.
	void computeMySegmentEndTangentVectors();

	void addSegment(	const float pointA_x, const float pointA_y, const float pointA_z, const int pointA_frame, const float pointA_U, const float pointA_V,
						const float pointB_x, const float pointB_y, const float pointB_z, const int pointB_frame, const float pointB_U, const float pointB_V);

	// Accessors.
	std::vector <ITTrajectoryCurveSegment*> *get_MyTrajectoryCurveSegments();
	void set_MyTrajectoryCurveSegments(std::vector <ITTrajectoryCurveSegment*> *v);

	std::vector <int> *get_MyFocusTrajectoryCurveSegmentIndices();
	void set_MyFocusTrajectoryCurveSegmentIndices(std::vector <int> *v);
};