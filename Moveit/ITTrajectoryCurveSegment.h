#pragma once

#include <vector>

// Forward declarations.
class ITPoint;
class ITPointTrajectory;

class ITTrajectoryCurveSegment
{
private:
	ITPointTrajectory *_P0_p; // Note that only the x component contains the data.
	ITPointTrajectory *_P1_p; // Note that only the x component contains the data.

	ITPoint *_m0_p;
	ITPoint *_m1_p;

	float _Tension;

    int _StartKeyFrame;
    int _EndKeyFrame;

	int _NoOfStrides;

	std::vector <float> *_ArcLengthIncrements;
	std::vector <float> *_ArcLengthCummulates;

	float _ArcLength;

public:
	ITTrajectoryCurveSegment(void);
	~ITTrajectoryCurveSegment(void);

	// Worker methods.
	ITPoint *evaluatePointAtParameterValue(float s);
	void calculateData();
	float calculateParameterForDistance(float D);

	// Accessors.
	ITPointTrajectory *get_P0_p();
	void set_P0_p(ITPointTrajectory *p);
	ITPointTrajectory *get_P1_p();
	void set_P1_p(ITPointTrajectory *p);

	ITPoint *get_m0_p();
	void set_m0_p(ITPoint *m);
	ITPoint *get_m1_p();
	void set_m1_p(ITPoint *m);

	float get_Tension();
	void set_Tension(float a);

    int get_StartKeyFrame();
    void set_StartKeyFrame(int a);
        
    int get_EndKeyFrame();
    void set_EndKeyFrame(int a);

	int get_NoOfStrides();
	void set_NoOfStrides(int n);

	std::vector <float> *get_ArcLengthIncrements();
	void set_ArcLengthIncrements(std::vector <float> *v);

	std::vector <float> *get_ArcLengthCummulates();
	void set_ArcLengthCummulates(std::vector <float> *v);

	float get_ArcLength();
	void set_ArcLength(float l);
};

