#pragma once
// Forward declaration.
class ITSurface;
class ITPoint;
class ITVortex;

class ITPanel
{

private:

	ITPoint* _bottomLeftPoint;
	ITPoint* _bottomRightPoint;
	ITPoint* _topLeftPoint;
	ITPoint* _topRightPoint;

	std::vector <ITVortex*> *_MyVortices; // The vortices actually on the panel.

	ITPoint* _leadingDiagonal;
	ITPoint* _trailingDiagonal;
	ITPoint* _midPoint;
	ITPoint* _colocationPoint;
	ITPoint* _MyMidPointEarthVelocity; // Fluctuates throughout the unsteady simulation.
	ITPoint* _Normal;

	float _Area;
	float _DeltaPressure; // Fluid dynamic pressure (Windward pressure minus leeward pressure).

	float _MyVorticity; // The vorticity of this panel. It gets set to the solPointer value.

	int _i; // row number.
	int _j; // column number.
	int _k; // surface number.

	bool _IgnoreForces; // When panel is inside the fuselage.

	bool _IsControlSurface; // Whether or not this ITPanel is part of a control surface.

protected:

	ITSurface* _MyITSurface; // The surface to which this panel belongs.

public:

	// Constructors.
	ITPanel(ITPoint* bottomLeft, ITPoint* bottomRight, ITPoint* topRight, ITPoint* topLeft, ITSurface* s, int typeOfPanel); // If typeOfPanel is 0 then we are dealing with a bound panel.
	~ITPanel(void);

	// Worker methods.
	void computeDiagonals();
	void computeMidPoint();
	void computeColocationPoint();
	void computeNormal();
	void computeArea();
	void instanciateMyVortices();

	void calculateMyTrajectoryVelocity(int k); // Gets called from ITPhysics.

											   // Accessors.
	ITSurface *get_MyITSurface();
	void set_MyITSurface(ITSurface *s);

	ITPoint *get_BottomLeftPoint();
	void set_BottomLeftPoint(ITPoint *p);

	ITPoint *get_BottomRightPoint();
	void set_BottomRightPoint(ITPoint *p);

	ITPoint *get_TopLeftPoint();
	void set_TopLeftPoint(ITPoint *p);

	ITPoint *get_TopRightPoint();
	void set_TopRightPoint(ITPoint *p);


	std::vector <ITVortex*> *get_MyVortices();
	void set_MyVortices(std::vector <ITVortex*> *v);


	ITPoint *get_LeadingDiagonal();
	void set_LeadingDiagonal(ITPoint *p);

	ITPoint *get_TrailingDiagonal();
	void set_TrailingDiagonal(ITPoint *p);

	ITPoint *get_MidPoint();
	void set_MidPoint(ITPoint *p);

	ITPoint *get_ColocationPoint();
	void set_ColocationPoint(ITPoint *p);

	ITPoint *get_MyMidPointEarthVelocity();
	void set_MyMidPointEarthVelocity(ITPoint *p);

	ITPoint *get_Normal();
	void set_Normal(ITPoint *p);

	float get_Area();
	void set_Area(float a);

	float get_DeltaPressure();
	void set_DeltaPressure(float f);

	float get_MyVorticity();
	void set_MyVorticity(float v);


	int get_I();
	void set_I(int i);

	int get_J();
	void set_J(int j);

	int get_K();
	void set_K(int k);

	bool get_IgnoreForces();
	void set_IgnoreForces(bool ignoreForces);

	bool get_IsControlSurface();
	void set_IsControlSurface(bool isControlSurface);


};