#pragma once
// Forward declaration.
class ITSurface;
class ITPoint;

class ITPanel
{

private:

	ITPoint* _bottomLeftPoint;
	ITPoint* _bottomRightPoint;
	ITPoint* _topLeftPoint;
	ITPoint* _topRightPoint;

	ITPoint* _leadingDiagonal;
	ITPoint* _trailingDiagonal;
	ITPoint* _midPoint;
	ITPoint* _colocationPoint;
	ITPoint* _Normal;

	float _Area;

	int _i; // row number.
	int _j; // column number.
	int _k; // surface number.

protected:

	ITSurface* _MyITSurface; // The surface to which this panel belongs.

public:

	// Constructors.
	ITPanel(ITPoint* bottomLeft, ITPoint* bottomRight, ITPoint* topRight, ITPoint* topLeft, ITSurface* s, int typeOfPanel); // If typeOfPanel is 0 then we are dealing with a bound panel.
	~ITPanel(void);

	// Worker methods.
	void computeDiagonals();
	void computeMidPoint();
	void computeNormal();
	void computeArea();

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

	ITPoint *get_LeadingDiagonal();
	void set_LeadingDiagonal(ITPoint *p);

	ITPoint *get_TrailingDiagonal();
	void set_TrailingDiagonal(ITPoint *p);

	ITPoint *get_MidPoint();
	void set_MidPoint(ITPoint *p);

    ITPoint *get_ColocationPoint();
    void set_ColocationPoint(ITPoint *p);

	ITPoint *get_Normal();
	void set_Normal(ITPoint *p);

	float get_Area();
	void set_Area(float a);



	int get_I();
	void set_I(int i);

	int get_J();
	void set_J(int j);

	int get_K();
	void set_K(int k);

};