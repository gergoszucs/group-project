#pragma once

#include <vector>

class ITTrajectoryCurveSegment;

class ITTrajectoryCurve
{
private:
	std::vector <ITTrajectoryCurveSegment*> *_MyTrajectoryCurveSegments;

public:
	ITTrajectoryCurve(void);
	~ITTrajectoryCurve(void);

	// Worker methods.
	void computeMySegmentEndTangentVectors();

	// Accessors.
	std::vector <ITTrajectoryCurveSegment*> *get_MyTrajectoryCurveSegments();
	void set_MyTrajectoryCurveSegments(std::vector <ITTrajectoryCurveSegment*> *v);
};