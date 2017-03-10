#include <cuda_runtime.h> 

__global__ void kernelInfluenceCoefficient(
	const float *px,
	const float *py,
	const float *pz,
	const float *nx,
	const float *ny,
	const float *nz,
	const float *vs_x,
	const float *vs_y,
	const float *vs_z,
	const float *ve_x,
	const float *ve_y,
	const float *ve_z,
	float *A,
	int noOfUnknownVortexStrengths,
	char rankineAlgorithmIndex,
	float RankineCoreRadius,
	int FrameNumber);

__host__ __device__ void kernelCalcFiniteSvensonAt(
	const float *px, // the coordinates of the object panel control point (the target point).
	const float *py, // the coordinates of the object panel control point (the target point).
	const float *pz, // the coordinates of the object panel control point (the target point).
	const float *vs_x, // The vortex start point coordinates.
	const float *vs_y, // The vortex start point coordinates.
	const float *vs_z, // The vortex start point coordinates.
	const float *ve_x, // The vortex end point coordinates.
	const float *ve_y, // The vortex end point coordinates.
	const float *ve_z, // The vortex end point coordinates.
	float *dvx, // Returned variables; the cartesian components of the velocity induced at the object panel by the current finite vortex on the subject panel.
	float *dvy, // Returned variables; the cartesian components of the velocity induced at the object panel by the current finite vortex on the subject panel.
	float *dvz // Returned variables; the cartesian components of the velocity induced at the object panel by the current finite vortex on the subject panel.
);

__global__ void kernelFunctionPredictVelocityAtPoint(
	const float *px,
	const float *py,
	const float *pz,
	const float *vs_x,
	const float *vs_y,
	const float *vs_z,
	const float *ve_x,
	const float *ve_y,
	const float *ve_z,
	float *p_vx,
	float *p_vy,
	float *p_vz,
	const float *vorticities,
	int noOfVorticesPerPanel,
	int noOfPanels,
	int noOfElementsInThisThread, // Stride.
	int noOfVelocityPredictions,
	int threadsPerTranche,
	int indexOfStartOfTranche,
	char rankineAlgorithmIndex);