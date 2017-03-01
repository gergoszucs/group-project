#pragma once

// System includes.
#include <vector>

class ITPhysics
{
public:
	ITPhysics(void);
	~ITPhysics(void);

	static void playOutStep();
	static void playOutFlexit();
	static void playOutDryRun();
	static void playOutReplay();
	static void playOutTest();

	static void delay(int millisecondsToWait);

	static int multiplyArbitraryMatrices(int rA, int cA, char storageModeA, float*A, int rB, int cB, char storageModeB, float*B, float*C);
	static void printThreeByThreeMatrix(int r, int c, char storageModeA, float*A, std::string title, int debugLevel);
	static void printDebugMatrix(int r, int c, char storageModeA, float*A, std::string title, int debugLevel);

private:

	static void prepareForSimulation();
	static void PropagateFlexit(int FrameNumber);

	static void CreateMatrixAndSolveForVortexStrengths(int FrameNumber);
	static void ManageCalculationOfMatrixOfCoefficients(std::vector <float> *solPointer, int noOfUnknownVortexStrengths);
	static void ManageCalculationOfVectorB(int noOfUnknownVortexStrengths);
	static void SolveForSol(std::vector <float> *solPointer, int noOfUnknownVortexStrengths);
	static void DoolittleWallstedt(int d, float *S, float *D);
	static void separateDoolittleLandU(int d, float *LU, float *L, float *U);
	static void TwoStageGaussianEliminationCPU(float *L, float *U, int noOfPanels, std::vector <float> *solPointer);
	static void PropagatePressureDistributionAndForces();
	static void PropagateSurfaceGeometry(int FrameNumber);
	static void InsertWakePanels(int FrameNumber);
	static void PropagateWakeGeometry(int FrameNumber);
	static void calcVelocityFieldCuda(int t);
	static void StoreDataForReplay();
	static void ITPhysics::calcInducedVelocityAtEdgeMidPointCuda(int k, int i, int j, int t, float x, float y, float z, float *vx, float *vy, float *vz);

};
