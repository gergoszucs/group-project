#pragma once

class ITPoint;
class ITPanel;

class ITVortex
{

protected:

	ITPoint* _startPoint;
	ITPoint* _endPoint;
	ITPanel* _MyITPanel;

	float _myLength;
	float _myLengthSquared;

public:

	ITVortex(ITPoint* startPoint, ITPoint* endPoint);
	~ITVortex(void) {};

	// Worker methods.
	ITPoint* calcFiniteSvensonAt(ITPoint *targetPoint);
	void calculateMyLength(void);

	// Accessors.
	ITPoint *get_StartPoint();
	void set_StartPoint(ITPoint *p);

	ITPoint *get_EndPoint();
	void set_EndPoint(ITPoint *p);

	ITPanel* get_MyITPanel();
	void set_MyITPanel(ITPanel *p);

	float get_MyLength();
	void set_MyLength(float a);

	float get_MyLengthSquared();
	void set_MyLengthSquared(float a);
};