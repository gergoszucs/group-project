#pragma once

// System includes.
#include <vector>

class ITPhysics
{
public:
	ITPhysics(void);
	~ITPhysics(void);

	static void playOutDryRun();
	static void delay( int millisecondsToWait );
	static void PropagateSurfaceGeometry(int FrameNumber);

private:


};
