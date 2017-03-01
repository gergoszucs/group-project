#pragma once

#include <string>
#include "ITPoint.h"

class ITBeamNode : public ITPoint
{

private:

	std::string _Name;

	ITPoint* _q; // The current applied load vector.
	ITPoint* _W; // The current linear perpendicular deflection vector (in the beam local coordinate system).
	ITPoint* _Wlast; // The last linear perpendicular deflection vector (in the beam local coordinate system).
	ITPoint* _Wnext; // The next linear perpendicular deflection vector (in the beam local coordinate system).
	float _Mass;
	float _MassPerUnitLength;
	float _RotationAngle; // Used in torque analysis.

						  // The following three instance variables are the values of E and I of the beam at the point where it passes though the node.
	float _E; // Youngs Modulus.
	float _Ix; // Second moment of area opposing rotation parallel to the x-axis.
	float _Iz; // Second moment of area opposing rotation parallel to the z-axis.

public:

	ITBeamNode(float x, float y, float z);
	~ITBeamNode(void);

	// Utilities.

	// IO


	// Accessors.	
	std::string get_Name();
	void set_Name(std::string s);

	ITPoint *get_W();
	void set_W(ITPoint *w);

	ITPoint *get_Wlast();
	void set_Wlast(ITPoint *w);

	ITPoint *get_Wnext();
	void set_Wnext(ITPoint *w);

	ITPoint *get_q();
	void set_q(ITPoint *q);

	float get_Mass();
	void set_Mass(float m);

	float get_MassPerUnitLength();
	void set_MassPerUnitLength(float m);




	float get_E();
	void set_E(float e);

	float get_Ix();
	void set_Ix(float i);

	float get_Iz();
	void set_Iz(float i);

	float get_RotationAngle();
	void set_RotationAngle(float r);


protected:


};