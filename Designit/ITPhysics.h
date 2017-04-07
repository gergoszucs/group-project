#pragma once

#include <vector>

class ITPhysics
{
public:
	ITPhysics(void) {};
	~ITPhysics(void) {};

	static int multiplyArbitraryMatrices(int rA, int cA, char storageModeA, float*A, int rB, int cB, char storageModeB, float*B, float*C);
	static void printDebugMatrix(int r, int c, char storageModeA, float*A, std::string title, int debugLevel);
	
	static void playOutDryRun();
	static void delay(int millisecondsToWait);
	static void PropagateSurfaceGeometry(int FrameNumber);
};