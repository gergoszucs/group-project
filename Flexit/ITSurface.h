#pragma once

#include <vector>
#include <Eigen/Dense>

class ITPoint;
class ITControlPoint;
class ITTrajectoryCurve;
class ITPanel;
class ITControlSurface;
class ITWakePanel;
class ITBeam;

class ITSurface
{
private:

	// ITSurface instance variables.
	int _NoOfInterpolatedPointsU;
	int _NoOfInterpolatedPointsV;

	std::vector < std::vector <ITControlPoint*> > *_MyControlPoints; // The control points.
	std::vector <ITTrajectoryCurve*> *_MyTrajectoryCurves; // A vector of pointers to six "new" 2D trajectory curves - one for each of x, y, z, roll, pitch and yaw.
	std::vector < std::vector <ITPanel*> > *_MyPanels; // The panels used in VLM.
	std::vector < std::vector <ITWakePanel*> > *_MyWakePanels; // The panels used in VLM.
	std::vector <ITControlSurface*> *_MyControlSurfaces; // The control surfaces belonging to this ITSurface.

	std::vector <int> *_MyAncestorSurfaceIndices; // A vector of ordered indices of surfaces that are my ancestors. Oldest one is at the end.

	// Histories.
	std::vector < std::vector < std::vector <ITWakePanel*> > > *_MyWakePanelHistory; // The history of wake panels used in VLM.
	std::vector < std::vector < std::vector <ITPoint*> > > *_MyControlPointDeflectionHistory; // The history of deflection of the surface control points.
	std::vector < std::vector < std::vector <float> > > *_MyPressureHistory; // The history of surface panel pressures used in VLM.
	std::vector < std::vector < std::vector <float> > > *_MyVorticityHistory; // The history of surface panel vorticities used in VLM.
	std::vector <float> *_MyCLHistory; // The history of surface CL used in VLM.
	std::vector <float> *_MyCDHistory; // The history of surface CD used in VLM.
	std::vector <ITPoint*> *_MyForceHistory; // The history of surface force vector fluid-dynamic.

	// Morphing information.
	bool _IsMorph; // Will we morph this surface?
	int _MorphStartFrame;
	int _MorphEndFrame;
	std::string _MorphType; // "Linear" or "Cosine".

	bool _IsWake; // True if the surface has a wake.
	bool _IsPistolesiPanelOffset; // True if we should apply 1/4-3/4 panel rule.
	float _PistolesiPanelOffsetFraction; // Normally this would be set to 0.25.
	float _FuselageRadius;
	int _ParentSurfaceIndex; // Used when project _IsSurfaceHierarchy is true. (set to -1 when not used).

	ITPoint* _MyCentreOfRotationPoint; // The point about which trajectory rotations take place.

	ITPoint* _MyCurrentTranslationPoint; // The point on the translation trajectory curve corresponding to the current FrameNumber.
	ITPoint* _MyCurrentRotationPoint; // The point on the rotation trajectory curve corresponding to the current FrameNumber.

	ITPoint* _MyPreviousTranslationPoint; // The point on the translation trajectory curve corresponding to the previous FrameNumber.
	ITPoint* _MyPreviousRotationPoint; // The point on the rotation trajectory curve corresponding to the previous FrameNumber.

	ITBeam* _MyBeam; // Aero-elastics.

	// Instance variables associated with bezier surface computations.
	Eigen::MatrixXd _MU;
	Eigen::MatrixXd _MV;

	std::vector <float> *_URange;
	std::vector <float> *_VRange;

	Eigen::MatrixXd _UPowerMatrix;
	Eigen::MatrixXd _VPowerMatrix;

	std::vector < std::vector <ITPoint*> > *_MyInterpolatedPoints; // The interpolated points computed using the Bezier algorithms (used by all applications).
	std::vector < std::vector <ITPoint*> > *_MyInterpolatedNormals; // The (unit) normal vectors corresponding to each interpolated point.

	// Utility methods.
	void computeMU();
	void computeMV();
	int factorial(int n);
	void computeURange();
	void computeVRange();
	void computeUPowerMatrix();
	void computeVPowerMatrix();
	void computeInterpolatedPoints();

	// Worker methods.
	void managePropagationOfPanels();
	void propagateWakePanelPointBasedOnCudaResults(ITPoint *tempPt, ITPoint *p, float vx, float vy, float vz, float DELTA_T);

public:

	ITSurface(void);
	~ITSurface(void);

	// Worker methods.
	void computeTrajectoryPointsAtFrame(int myFrameNumber, int mySurfaceIndex, ITPoint* translationPoint, ITPoint* rotationPoint);
	void computeNetTrajectoryPointsAtFrame(int myFrameNumber, int mySurfaceIndex, ITPoint* translationPoint, ITPoint* rotationPoint);

	// Miscellaneous.
	void prepareForSimulation();
	void moveMeBackToBase(int k);

	// Utilities.
	void manageComputationOfInterpolatedPoints();

	// Flexit worker methods.
	void instanciateMyPanels(int mySurfaceIndex);
	void propagateGeometry(int k);
	void insertWakePanels();
	void propagateWakeGeometryCuda(int k);

	// Accessors.	
	std::vector < std::vector <ITControlPoint*> > *get_MyControlPoints();
	void set_MyControlPoints(std::vector < std::vector <ITControlPoint*> > *myControlPoints);

	std::vector <ITTrajectoryCurve*> *get_MyTrajectoryCurves();
	void set_MyTrajectoryCurves(std::vector <ITTrajectoryCurve*> *myTrajectoryCurves);

	std::vector <int> *get_MyAncestorSurfaceIndices();
	void set_MyAncestorSurfaceIndices(std::vector <int> *myAncestorSurfaceIndices);

	std::vector < std::vector <ITPanel*> > *get_MyPanels();
	void set_MyPanels(std::vector < std::vector <ITPanel*> > *myPanels);

	std::vector < std::vector <ITWakePanel*> > *get_MyWakePanels();
	void set_MyWakePanels(std::vector < std::vector <ITWakePanel*> > *myWakePanels);

	std::vector <ITControlSurface*> *get_MyControlSurfaces();
	void set_MyControlSurfaces(std::vector <ITControlSurface*> *a);

	std::vector < std::vector <ITPoint*> > *get_MyInterpolatedPoints();
	void set_MyInterpolatedPoints(std::vector < std::vector <ITPoint*> > *myInterpolatedPoints);

	std::vector < std::vector <ITPoint*> > *get_MyInterpolatedNormals();
	void set_MyInterpolatedNormals(std::vector < std::vector <ITPoint*> > *myInterpolatedNormals);

	// Histories.
	std::vector < std::vector < std::vector <ITWakePanel*> > > *get_MyWakePanelHistory();
	void set_MyWakePanelHistory(std::vector < std::vector < std::vector <ITWakePanel*> > > *myWakePanelHistory);

	std::vector < std::vector < std::vector <ITPoint*> > > *get_MyControlPointDeflectionHistory();
	void set_MyControlPointDeflectionHistory(std::vector < std::vector < std::vector <ITPoint*> > > *myControlPointDeflectionHistory);

	std::vector < std::vector < std::vector <float> > > *get_MyPressureHistory();
	void set_MyPressureHistory(std::vector < std::vector < std::vector <float> > > *myPressureHistory);

	std::vector < std::vector < std::vector <float> > > *get_MyVorticityHistory();
	void set_MyVorticityHistory(std::vector < std::vector < std::vector <float> > > *myVorticityHistory);

	std::vector <float> *get_MyCLHistory();
	void set_MyCLHistory(std::vector <float> *myCLHistory);

	std::vector <float> *get_MyCDHistory();
	void set_MyCDHistory(std::vector <float> *myCDHistory);

	std::vector <ITPoint*> *get_MyForceHistory();
	void set_MyForceHistory(std::vector <ITPoint*> *f);

	int get_NoOfInterpolatedPointsU();
	void set_NoOfInterpolatedPointsU(int u);

	int get_NoOfInterpolatedPointsV();
	void set_NoOfInterpolatedPointsV(int v);

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

	ITPoint *get_MyCentreOfRotationPoint();
	void set_MyCentreOfRotationPoint(ITPoint* p);

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

	ITPoint *get_MyCurrentTranslationPoint();
	void set_MyCurrentTranslationPoint(ITPoint* p);

	ITPoint *get_MyCurrentRotationPoint();
	void set_MyCurrentRotationPoint(ITPoint* p);

	ITPoint *get_MyPreviousTranslationPoint();
	void set_MyPreviousTranslationPoint(ITPoint* p);

	ITPoint *get_MyPreviousRotationPoint();
	void set_MyPreviousRotationPoint(ITPoint* p);

	ITBeam *get_MyBeam();
	void set_MyBeam(ITBeam* b);
};