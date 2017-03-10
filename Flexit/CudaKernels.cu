#include "cuda_runtime.h"
#include "device_launch_parameters.h" // blockIdx gridDim etc.
#include "qmath.h"

__host__ __device__ void kernelCalcFiniteSvensonAt(const float *px, // the coordinates of the object panel control point (the target point).
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
)
{
	// This method returns a prediction of the velocity (in a ITPoint object) induced at the target point by this vortex.

	// Reference: 1. Sofia Werner MSc Report.
	//				 /Users/dominiquefleischmann/Documents/WorkingFolder/SWDevelopment/PCMAC/C++/Surfit Family Dev/Stuff/Documentation/sofiawernerMscreport.pdf

	/* This function computes the velocity at the point targetPoint induced by this
	* finite straight-line vortex segment using the closed form expression
	* due to Svenson and presented by Sofia Werner in Appendix A of her 2001 thesis.
	* Checked 20120708.
	*/

	// 20170203: Included denominator 4 PI.
	// Note that the induced velocity formulation used here excludes the multiplicative factor 1/(4 PI).
	float a, b, c, dx, dy, dz, term, sqrtABC, sqrtAterm, a1;

	// a is the square of the distance from the vortex start point to the object (target) point.
	// Since we are usually using a panel mid-point as a target point, the variable a will generally be greater than 0.
	a = (*px - *vs_x)*(*px - *vs_x)
		+ (*py - *vs_y)*(*py - *vs_y)
		+ (*pz - *vs_z)*(*pz - *vs_z);

	b = -2 * (
		(*ve_x - *vs_x)*(*px - *vs_x)
		+ (*ve_y - *vs_y)*(*py - *vs_y)
		+ (*ve_z - *vs_z)*(*pz - *vs_z)
		);

	// DOM: The length of the vortex squared.
	c = (*ve_x - *vs_x)*(*ve_x - *vs_x) + (*ve_y - *vs_y)*(*ve_y - *vs_y) + (*ve_z - *vs_z)*(*ve_z - *vs_z);

	dx = (*ve_z - *vs_z)*(*py - *vs_y)
		- (*ve_y - *vs_y)*(*pz - *vs_z);

	dy = (*ve_x - *vs_x)*(*pz - *vs_z)
		- (*ve_z - *vs_z)*(*px - *vs_x);

	dz = (*ve_y - *vs_y)*(*px - *vs_x)
		- (*ve_x - *vs_x)*(*py - *vs_y);

	term = 4 * a*c - b*b;

	if (term == 0.0) // Denominator.
	{
		*dvx = 0.0;
		*dvy = 0.0;
		*dvz = 0.0;

		return;
	}

	if (a <= 0.0) // Square of a denominator.
	{
		*dvx = 0.0;
		*dvy = 0.0;
		*dvz = 0.0;

		return;
	}

	if (a + b + c < 0.0) // Square of a denominator.
	{
		*dvx = 0.0;
		*dvy = 0.0;
		*dvz = 0.0;

		return;
	}

	sqrtABC = sqrtf(a + b + c);
	sqrtAterm = b / sqrtf(a);
	a1 = -2 * ((2 * c + b) / sqrtABC - sqrtAterm) / term;

	*dvx = dx * a1;
	*dvy = dy * a1;
	*dvz = dz * a1;

	// FIXME: This is a cludge to avoid NaN problems in the calling function. Not sure why this helps.
	if ((fabs(*dvx) > 1000.0) || (fabs(*dvy) > 1000.0) || (fabs(*dvz) > 1000.0))
	{
		*dvx = 0.0;
		*dvy = 0.0;
		*dvz = 0.0;

		return;
	}

	//if (custom_isnan(*dvx) || custom_isnan(*dvy) || custom_isnan(*dvz))
	//{
	//	*dvx = 0.0;
	//	*dvy = 0.0;
	//	*dvz = 0.0;

	//	cuPrintfDebug(2, "******** NAN\n", "", "", "", "", "", "");

	//	return;
	//}

	return;
} // End of kernelCalcFiniteSvensonAt.

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
	int FrameNumber)
{
	// An instance of this kernel gets instanciated for each entry in the A matrix of influence coefficients.

	// =================================================================================
	// Input parameters:
	// px - the x-coordinate of the object panel control point.
	// py - the y-coordinate of the object panel control point.
	// pz - the z-coordinate of the object panel control point.
	// nx - the x-coordinate of the unit normal vector of the object panel.
	// ny - the y-coordinate of the unit normal vector of the object panel.
	// nz - the z-coordinate of the unit normal vector of the object panel.
	// vs_x - finite vortex start point.
	// vs_y - finite vortex start point.
	// vs_z - finite vortex start point.
	// ve_x - finite vortex end point.
	// ve_y - finite vortex end point.
	// ve_z - finite vortex end point.
	// noOfUnknownVortexStrengths - the row and column size of the matrix A.

	// Output parameter:
	// A - the entry in the A matrix corresponding to the index of this kernel instance.
	// =================================================================================

	// blockDim.x = number of threads in the block.
	// blockIdx.x = block index.
	// threadIdx.x = the thread index within the current block.

	int index = blockIdx.y  * gridDim.x  * blockDim.z * blockDim.y * blockDim.x
		+ blockIdx.x  * blockDim.z * blockDim.y * blockDim.x
		+ threadIdx.z * blockDim.y * blockDim.x
		+ threadIdx.y * blockDim.x
		+ threadIdx.x;

	if (index < noOfUnknownVortexStrengths*noOfUnknownVortexStrengths)  // DOM: Only do the computation if the index is valid.
																		// DOM: This conditional is needed in case the number of instances
																		//      of the kernel multiplied by noOfElementsInThisThread is greater than
																		//      the number of panels squared.
	{

		// DOM: Find the entry in the A matrix corresponding to the current value of index.
		// DOM: Note that in transient computations the b vector includes contributions from the wake.
		//      The b vector is computed in other code.
		int row = index / noOfUnknownVortexStrengths; // DOM: Object panel i. This is the row of the A matrix.
		int col = index % noOfUnknownVortexStrengths; // DOM: Subject panel j. This is the column of the A matrix.

		int maxNoOfVortices = 4; // This hard-coded number should be identical to the hard coded number on line 130 of ManageCalculationOfMatrixOfCoefficients in ITPhysics.cpp.

								 // DOM: Initialize the velocity induced at the row-th surface object panel due to the col-th surface subject panel.
		float Vx = 0.0;
		float Vy = 0.0;
		float Vz = 0.0;

		// DOM: Loop over all the finite vortices on the surface subject panel (4 segments on each panel).
		// DOM: Note that the contribution from the wake quadrilateral vortex loops is accounted for in the b vector.
		for (int n = 0; n < maxNoOfVortices; n++)
		{
			float dvx = 0.0;
			float dvy = 0.0;
			float dvz = 0.0;

			// 20160616.
			// Test for the zero vector.
			float epsilon = 0.001;
			if ((fabs(vs_x[col*maxNoOfVortices + n] - ve_x[col*maxNoOfVortices + n]) < epsilon)
				&& (fabs(vs_y[col*maxNoOfVortices + n] - ve_y[col*maxNoOfVortices + n]) < epsilon)
				&& (fabs(vs_z[col*maxNoOfVortices + n] - ve_z[col*maxNoOfVortices + n]) < epsilon))
			{
				// Points are (almost) colinear, so return with dv set to zero.
				return;
			}

			// Calculate the induced velocity at the colocation point of the object panel due to all the finite vortices on the current subject panel.
			// Use Biot-Savart vortex with Rankine viscous core.
			kernelCalcFiniteSvensonAt(
				&px[row],
				&py[row],
				&pz[row],
				&vs_x[col*maxNoOfVortices + n],
				&vs_y[col*maxNoOfVortices + n],
				&vs_z[col*maxNoOfVortices + n],
				&ve_x[col*maxNoOfVortices + n],
				&ve_y[col*maxNoOfVortices + n],
				&ve_z[col*maxNoOfVortices + n],
				&dvx,
				&dvy,
				&dvz);

			// DOM: Add the contribution to the induced velocity due
			// to the current vortex in the subject
			// panel vortex loop.
			Vx = Vx + dvx;
			Vy = Vy + dvy;
			Vz = Vz + dvz;
		}
		  // Now compute A[ index ].
		  // This is the dot-product of the
		  // induced velocity with the unit normal at 
		  // the target point (colocation point of the object panel).
		A[index] = (nx[row] * Vx + ny[row] * Vy + nz[row] * Vz);
	}
}

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
	char rankineAlgorithmIndex)
{
	// Input parameters:
	// px - the x-coordinate of the object panel control point.
	// py - the y-coordinate of the object panel control point.
	// pz - the z-coordinate of the object panel control point.
	// vs_x - finite vortex start point.
	// vs_y - finite vortex start point.
	// vs_z - finite vortex start point.
	// ve_x - finite vortex end point.
	// ve_y - finite vortex end point.
	// ve_z - finite vortex end point.
	// vorticities - the values of vorticities on each panel.

	// Output parameters:
	// p_vx - The x-components of velocity at each object point.
	// p_vy - The y-components of velocity at each object point.
	// p_vz - The z-components of velocity at each object point.

	// maxNoOfFiniteVorticesPerPanel = maxNoOfVortices

	// blockDim.x = number of threads in the block.
	// blockIdx.x = block index.
	// threadIdx.x = the thread index within the current block.

	// Each instance of this kernel function is responsible for predicting the induced velocity at a single object point
	// as the result of the sum of all the vortex segments.

	// Be careful about evaluating the velocity near a vortex.

	// The index of the object point is threadIndex.

	int threadIndex = blockIdx.y  * gridDim.x  * blockDim.z * blockDim.y * blockDim.x
		+ blockIdx.x  * blockDim.z * blockDim.y * blockDim.x
		+ threadIdx.z * blockDim.y * blockDim.x
		+ threadIdx.y * blockDim.x
		+ threadIdx.x;

	// DOM: Initialize the free stream velocity vector.
	float Vx = 0.0f;
	float Vy = 0.0f;
	float Vz = 0.0f;

	// DOM: Only do the computation if we are in a valid thread.
	if (threadIndex < noOfVelocityPredictions)
	{
		// DOM: Only do the computation if the threadIndex is in this tranche.
		if ((threadIndex >= indexOfStartOfTranche) && (threadIndex < (indexOfStartOfTranche + threadsPerTranche)))
		{
			// DOM: Loop through the panels in the project.
			for (int panelIndex = 0; panelIndex < noOfPanels; panelIndex++) // Loop through all the subject panels.
			{
				// DOM: Loop through the vortices of the current panel.
				for (int vortexIndex = 0; vortexIndex < noOfVorticesPerPanel; vortexIndex++)
				{
					// Initialize the velocity induced by the current vortex at the object point.
					float dvx = 0.0;
					float dvy = 0.0;
					float dvz = 0.0;

					// DOM: Actually calculate the velocity induced at the object point due to the current vortex.
					// ROW_MAJOR_IDX2C(panelIndex, vortexIndex, maxNoOfVortices)
					// Avoid computing Svendsen induced velocity for object points close to the current vortex.
					float dsxSquared = (vs_x[panelIndex*noOfVorticesPerPanel + vortexIndex] - px[threadIndex]) * (vs_x[panelIndex*noOfVorticesPerPanel + vortexIndex] - px[threadIndex]);
					float dsySquared = (vs_y[panelIndex*noOfVorticesPerPanel + vortexIndex] - py[threadIndex]) * (vs_y[panelIndex*noOfVorticesPerPanel + vortexIndex] - py[threadIndex]);
					float dszSquared = (vs_z[panelIndex*noOfVorticesPerPanel + vortexIndex] - pz[threadIndex]) * (vs_z[panelIndex*noOfVorticesPerPanel + vortexIndex] - pz[threadIndex]);

					float dsSquared = dsxSquared + dsySquared + dszSquared;

					float dexSquared = (ve_x[panelIndex*noOfVorticesPerPanel + vortexIndex] - px[threadIndex]) * (ve_x[panelIndex*noOfVorticesPerPanel + vortexIndex] - px[threadIndex]);
					float deySquared = (ve_y[panelIndex*noOfVorticesPerPanel + vortexIndex] - py[threadIndex]) * (ve_y[panelIndex*noOfVorticesPerPanel + vortexIndex] - py[threadIndex]);
					float dezSquared = (ve_z[panelIndex*noOfVorticesPerPanel + vortexIndex] - pz[threadIndex]) * (ve_z[panelIndex*noOfVorticesPerPanel + vortexIndex] - pz[threadIndex]);

					float deSquared = dexSquared + deySquared + dezSquared;

					float eps = 0.0005;
					if ((dsSquared < eps) || (deSquared < eps))
					{
						// Object point is very close to one of the ends of the vortex, so leave induced velocity unchanged as zero. 
					}
					else
					{
						// Pass the addresses of array entries as parameters.
						kernelCalcFiniteSvensonAt(
							&px[threadIndex],
							&py[threadIndex],
							&pz[threadIndex],
							&vs_x[panelIndex*noOfVorticesPerPanel + vortexIndex],
							&vs_y[panelIndex*noOfVorticesPerPanel + vortexIndex],
							&vs_z[panelIndex*noOfVorticesPerPanel + vortexIndex],
							&ve_x[panelIndex*noOfVorticesPerPanel + vortexIndex],
							&ve_y[panelIndex*noOfVorticesPerPanel + vortexIndex],
							&ve_z[panelIndex*noOfVorticesPerPanel + vortexIndex],
							&dvx,
							&dvy,
							&dvz);
					}

					Vx = Vx + dvx*vorticities[panelIndex];
					Vy = Vy + dvy*vorticities[panelIndex];
					Vz = Vz + dvz*vorticities[panelIndex];
				}
			}

			p_vx[threadIndex] = Vx;
			p_vy[threadIndex] = Vy;
			p_vz[threadIndex] = Vz;
		}
	}
}