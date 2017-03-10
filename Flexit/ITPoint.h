#pragma once

#include <string>
#include "rapidjson/document.h" // https://github.com/miloyip/rapidjson

class ITPoint
{

private:

	float _X;
	float _Y;
	float _Z;

	float _VX;
	float _VY;
	float _VZ;

	float _Mass;

	int _i; // row number.
	int _j; // column number.
	int _k; // Parent surface index.

	std::string _Name;

	float _U; // Intrinsic coordinate.
	float _V; // Intrinsic coordinate.

public:

	ITPoint(float x, float y, float z);
	ITPoint(void) {};
	~ITPoint(void);

	// Utilities.
	void normalize();
	float magnitude();
	float dot(ITPoint *otherPt);
	ITPoint *cross(ITPoint *otherPt, ITPoint *result);

	// Worker methods.
	void propagateMeWithMorph(ITPoint* cp, ITPoint* rotationPoint, ITPoint* translationPoint, bool isMorph, int startFrame, int endFrame, std::string typeOfMoph);
	void propagateMeWithMorphForTrajectorySpeed(ITPoint* cp, ITPoint* rotationPoint, ITPoint* translationPoint, bool isMorph, int startFrame, int endFrame, std::string typeOfMoph, int k, int myFrameNumber);
	float distanceFrom(ITPoint* secondPoint);

	// IO
	void serializeMeAsJSONObject(int k, int i, int j, rapidjson::Value *controlPointsArray, rapidjson::Document *d);

	// Accessors.	
	float get_X();
	void set_X(float x);

	float get_Y();
	void set_Y(float y);

	float get_Z();
	void set_Z(float z);

	float get_VX();
	void set_VX(float vx);

	float get_VY();
	void set_VY(float vy);

	float get_VZ();
	void set_VZ(float vz);

	float get_Mass();
	void set_Mass(float m);

	int get_I();
	void set_I(int i);

	int get_J();
	void set_J(int j);

	int get_K();
	void set_K(int k);

	std::string get_Name();
	void set_Name(std::string s);

	float get_U();
	void set_U(float u);

	float get_V();
	void set_V(float v);
};