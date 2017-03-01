#pragma once
// Forward declaration.

class ITGust
{

private:


protected:

	float _X0;
	float _Y0;
	float _Z0;
	float _SD;
	float _MaxSpeed;

public:

	// Constructors.
	ITGust(float x0, float y0, float z0, float SD, float maxSpeed);
	~ITGust(void);

	// Worker methods.
	void computeVelocityAtPoint(float x, float y, float z, float *vx, float *vy, float *vz);

	// Accessors.
	float get_X0();
	void set_X0(float x);

	float get_Y0();
	void set_Y0(float y);

	float get_Z0();
	void set_Z0(float z);

	float get_SD();
	void set_SD(float sd);

	float get_MaxSpeed();
	void set_MaxSpeed(float s);

};