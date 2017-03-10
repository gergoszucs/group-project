#pragma once

#include <vector>
#include <Eigen/Dense>
#include "rapidjson/document.h" // https://github.com/miloyip/rapidjson

// Forward declarations.
class ITPoint;
class ITControlPoint;

class ITSurface
{
private:

	std::vector < std::vector <ITControlPoint*> > *_MyControlPoints; // The control points.
	std::vector <ITControlPoint*> *_MyFocusControlPoints; // Focus control points.

	// Instance variables associated with bezier surface computations.
	int _NoOfInterpolatedPointsU;
	int _NoOfInterpolatedPointsV;

	Eigen::MatrixXd _MU;
	Eigen::MatrixXd _MV;

	std::vector <float> *_URange;
	std::vector <float> *_VRange;

	Eigen::MatrixXd _UPowerMatrix;
	Eigen::MatrixXd _VPowerMatrix;

	std::vector < std::vector <ITPoint*> > *_MyInterpolatedPoints; // The interpolated points computed using the Bezier algorithms (used by all applications).
	std::vector < std::vector <ITPoint*> > *_MyInterpolatedNormals; // The (unit) normal vectors corresponding to each interpolated point.

	std::vector < std::vector <float> > *_MyGaussianCurvature;

	bool _IsMorph; // Will we morph this surface?
	int _MorphStartFrame;
	int _MorphEndFrame;
	std::string _MorphType; // "Linear" or "Cosine".
	bool _IsWake; // True if the surface has a wake.
	bool _IsPistolesiPanelOffset; // True if we should apply 1/4-3/4 panel rule.
	float _PistolesiPanelOffsetFraction; // Normally this would be set to 0.25.
	float _FuselageRadius;
	int _ParentSurfaceIndex; // Used when project _IsSurfaceHierarchy is true. (set to -1 when not used).

	// Utility methods.
	void computeMU();
	void computeMV();
	int factorial(int n);
	void computeURange();
	void computeVRange();
	void computeUPowerMatrix();
	void computeVPowerMatrix();
	void computeInterpolatedPoints();

public:

	ITSurface(void);
	~ITSurface(void);

	// Utilities.
	void manageComputationOfInterpolatedPoints();

	// Accessors.
	int get_NoOfInterpolatedPointsU();
	void set_NoOfInterpolatedPointsU(int u);

	int get_NoOfInterpolatedPointsV();
	void set_NoOfInterpolatedPointsV(int v);

	std::vector < std::vector <ITControlPoint*> > *get_MyControlPoints();
	void set_MyControlPoints(std::vector < std::vector <ITControlPoint*> > *myControlPoints);

	std::vector <ITControlPoint*> *get_MyFocusControlPoints();
	void set_MyFocusControlPoints(std::vector <ITControlPoint*> *myFocusControlPoints);

	std::vector < std::vector <ITPoint*> > *get_MyInterpolatedPoints();
	void set_MyInterpolatedPoints(std::vector < std::vector <ITPoint*> > *myInterpolatedPoints);

	std::vector < std::vector <ITPoint*> > *get_MyInterpolatedNormals();
	void set_MyInterpolatedNormals(std::vector < std::vector <ITPoint*> > *myInterpolatedNormals);

	std::vector < std::vector <float> > *get_MyGaussianCurvature();
	void set_MyGaussianCurvature(std::vector < std::vector <float> > *myGaussianCurvature);

	Eigen::MatrixXd get_MU();
	void set_MU(Eigen::MatrixXd mu);

	Eigen::MatrixXd get_MV();
	void set_MV(Eigen::MatrixXd mv);

	std::vector <float> *get_URange();
	void set_URange(std::vector <float> *ur);

	std::vector <float> *get_VRange();
	void set_VRange(std::vector <float> *vr);

	Eigen::MatrixXd get_UPowerMatrix();
	void set_UPowerMatrix(Eigen::MatrixXd upm);

	Eigen::MatrixXd get_VPowerMatrix();
	void set_VPowerMatrix(Eigen::MatrixXd vpm);

	bool get_IsMorph();
	void set_IsMorph(bool b);

	int get_MorphStartFrame();
	void set_MorphStartFrame(int f);

	int get_MorphEndFrame();
	void set_MorphEndFrame(int f);

	std::string get_MorphType();
	void set_MorphType(std::string s);

	bool get_IsWake();
	void set_IsWake(bool b);

	int get_ParentSurfaceIndex();
	void set_ParentSurfaceIndex(int f);

	bool get_IsPistolesiPanelOffset();
	void set_IsPistolesiPanelOffset(bool b);

	float get_PistolesiPanelOffsetFraction();
	void set_PistolesiPanelOffsetFraction(float p);

	float get_FuselageRadius();
	void set_FuselageRadius(float f);
};