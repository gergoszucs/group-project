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

	// Accessors.
	std::vector <ITTrajectoryCurveSegment*> *get_MyTrajectoryCurveSegments();
	void set_MyTrajectoryCurveSegments(std::vector <ITTrajectoryCurveSegment*> *v);

	std::vector <int> *get_MyFocusTrajectoryCurveSegmentIndices();
	void set_MyFocusTrajectoryCurveSegmentIndices(std::vector <int> *v);
};