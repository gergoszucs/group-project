#include <cuda_runtime.h> 
#include <cstdlib>
#include <iostream>
#include <time.h>

// Dom's includes.
#include "CudaPhysics.cuh"
#include "CudaKernels.cuh"

void ConstructMatrixOfInfluenceCoefficientsCuda(
	const float *h_cp_x,
	const float *h_cp_y,
	const float *h_cp_z,
	const float *h_n_x,
	const float *h_n_y,
	const float *h_n_z,
	const float *h_vs_x,
	const float *h_vs_y,
	const float *h_vs_z,
	const float *h_ve_x,
	const float *h_ve_y,
	const float *h_ve_z,
	float *h_A, // Output influence coefficient matrix.
	int noOfUnknownVortexStrengths, // This is basically the number of surface panels.
	float RankineCoreRadius,
	char rankineAlgorithmIndex,
	int FrameNumber)
{
	// DOM: Called from ManageCalculationOfMatrixOfCoefficients in ITPhysics.cpp.

	cudaError_t err; // Error code to check return values for CUDA calls.

	// DOM: Calculate the sizes of the arrays passed in to this function.
	int totalNumberOfCudaComputations = noOfUnknownVortexStrengths*noOfUnknownVortexStrengths; // This is the number of elements in the A matrix, and accounts for the influence of each panel on each panel.

	size_t sizeRowFloat = noOfUnknownVortexStrengths * sizeof(float); // Memory required for a row of floats.
	size_t sizeMatrixFloat = totalNumberOfCudaComputations * sizeof(float); // Memory required for a matrix of floats.

	int maxNoOfVortices = 4;

	// ============================================================================
	// Allocate the GPU memory.
	// ============================================================================
	// Colocation point coordinates.
	float *d_cp_x = NULL;
	err = cudaMalloc((void **)&d_cp_x, sizeRowFloat); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_cp_y = NULL;
	err = cudaMalloc((void **)&d_cp_y, sizeRowFloat); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_cp_z = NULL;
	err = cudaMalloc((void **)&d_cp_z, sizeRowFloat); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	// Panel Normals.
	float *d_n_x = NULL;
	err = cudaMalloc((void **)&d_n_x, sizeRowFloat); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_n_y = NULL;
	err = cudaMalloc((void **)&d_n_y, sizeRowFloat); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_n_z = NULL;
	err = cudaMalloc((void **)&d_n_z, sizeRowFloat); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	// Vortex end point coordinates.
	float *d_vs_x = NULL;
	err = cudaMalloc((void **)&d_vs_x, sizeRowFloat*maxNoOfVortices); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_vs_y = NULL;
	err = cudaMalloc((void **)&d_vs_y, sizeRowFloat*maxNoOfVortices); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_vs_z = NULL;
	err = cudaMalloc((void **)&d_vs_z, sizeRowFloat*maxNoOfVortices); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_ve_x = NULL;
	err = cudaMalloc((void **)&d_ve_x, sizeRowFloat*maxNoOfVortices); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_ve_y = NULL;
	err = cudaMalloc((void **)&d_ve_y, sizeRowFloat*maxNoOfVortices); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_ve_z = NULL;
	err = cudaMalloc((void **)&d_ve_z, sizeRowFloat*maxNoOfVortices); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	// The memory for the square matrix of influence coefficient entries.
	float *d_A = NULL;
	err = cudaMalloc((void **)&d_A, sizeMatrixFloat); if (err != cudaSuccess) { exit(EXIT_FAILURE); }

	// ============================================================================
	// Copy host memory to device memory.
	// ============================================================================
	err = cudaMemcpy(d_cp_x, h_cp_x, sizeRowFloat, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_cp_y, h_cp_y, sizeRowFloat, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_cp_z, h_cp_z, sizeRowFloat, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_n_x, h_n_x, sizeRowFloat, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_n_y, h_n_y, sizeRowFloat, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_n_z, h_n_z, sizeRowFloat, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_vs_x, h_vs_x, sizeRowFloat*maxNoOfVortices, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_vs_y, h_vs_y, sizeRowFloat*maxNoOfVortices, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_vs_z, h_vs_z, sizeRowFloat*maxNoOfVortices, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_ve_x, h_ve_x, sizeRowFloat*maxNoOfVortices, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_ve_y, h_ve_y, sizeRowFloat*maxNoOfVortices, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_ve_z, h_ve_z, sizeRowFloat*maxNoOfVortices, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }

	err = cudaMemcpy(d_A, h_A, sizeMatrixFloat, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }


	// ============================================================================
	// Call the Kernel.
	// ============================================================================
	int noOfElementsInEachThread = 1;
	int totalNumberOfThreads = (totalNumberOfCudaComputations + noOfElementsInEachThread - 1) / noOfElementsInEachThread;

	int threadsPerBlock = 256; // 256; // When running out of resources, try reduce the threadsPerBlock.
	int totalNumberOfBlocks = (totalNumberOfThreads + threadsPerBlock - 1) / threadsPerBlock;
	int noOfBlocksX = 64;
	int noOfBlocksY = (totalNumberOfBlocks + noOfBlocksX - 1) / noOfBlocksX;

	dim3 grid(noOfBlocksX, noOfBlocksY, 1);
	dim3 block(threadsPerBlock, 1, 1);

	// Call noOfBlocksX*noOfBlocksY*threadsPerBlock instances of the kernel.
	kernelInfluenceCoefficient << <grid, block >> >(
		d_cp_x,
		d_cp_y,
		d_cp_z,
		d_n_x,
		d_n_y,
		d_n_z,
		d_vs_x,
		d_vs_y,
		d_vs_z,
		d_ve_x,
		d_ve_y,
		d_ve_z,
		d_A,
		noOfUnknownVortexStrengths,
		rankineAlgorithmIndex,
		RankineCoreRadius,
		FrameNumber);

	// Synchronize the CUDA kernels.
	cudaDeviceSynchronize();

	// Deal with any errors.
	err = cudaGetLastError();
	if (err != cudaSuccess)
	{
		std::cout << "Failed to launch kernelInfluenceCoefficient kernel (error code " << cudaGetErrorString(err) << ")" << std::endl;
		exit(EXIT_FAILURE);
	}

	// ====================================================================================================
	// Copy the coefficient vector back from the GPU device.
	// Copy the device result vector in device memory to the host result vector in host memory.
	// ====================================================================================================
	err = cudaMemcpy(h_A, d_A, sizeMatrixFloat, cudaMemcpyDeviceToHost);


	// ============================================================================
	// Free the GPU memory.
	// ============================================================================
	err = cudaFree(d_cp_x); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_cp_y); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_cp_z); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_n_x); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_n_y); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_n_z); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_vs_x); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_vs_y); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_vs_z); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_ve_x); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_ve_y); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_ve_z); if (err != cudaSuccess) { exit(EXIT_FAILURE); }

	err = cudaFree(d_A); if (err != cudaSuccess) { exit(EXIT_FAILURE); }

}



void ComputeVelocitiesForBatchOfPointsCuda(
	const float *h_cp_x,
	const float *h_cp_y,
	const float *h_cp_z,
	const float *h_vs_x,
	const float *h_vs_y,
	const float *h_vs_z,
	const float *h_ve_x,
	const float *h_ve_y,
	const float *h_ve_z,
	float *h_cp_vx,
	float *h_cp_vy,
	float *h_cp_vz,
	const float *h_vorticities,
	int noOfVorticesPerPanel,
	int noOfSubjectPanels,
	int noOfVelocityPredictions,
	int rankineAlgorithmIndex)
{

	// DOM: Error code to check return values for CUDA calls.
	cudaError_t err;

	// DOM: Calculate the sizes of the arrays passed in to this function.
	size_t sizeSubjectPanelsFloat = noOfSubjectPanels * sizeof(float); // Memory required for a row of floats.
	size_t sizeVelocityPredictionsFloat = noOfVelocityPredictions * sizeof(float); // Memory required for noOfVelocityPredictions floats.

	// ============================================================================
	// Allocate the GPU memory.
	// ============================================================================
	// Object point coordinates.
	float *d_cp_x = NULL;
	err = cudaMalloc((void **)&d_cp_x, sizeVelocityPredictionsFloat); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_cp_y = NULL;
	err = cudaMalloc((void **)&d_cp_y, sizeVelocityPredictionsFloat); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_cp_z = NULL;
	err = cudaMalloc((void **)&d_cp_z, sizeVelocityPredictionsFloat); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	// Vortex end point coordinates.
	float *d_vs_x = NULL;
	err = cudaMalloc((void **)&d_vs_x, sizeSubjectPanelsFloat*noOfVorticesPerPanel); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_vs_y = NULL;
	err = cudaMalloc((void **)&d_vs_y, sizeSubjectPanelsFloat*noOfVorticesPerPanel); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_vs_z = NULL;
	err = cudaMalloc((void **)&d_vs_z, sizeSubjectPanelsFloat*noOfVorticesPerPanel); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_ve_x = NULL;
	err = cudaMalloc((void **)&d_ve_x, sizeSubjectPanelsFloat*noOfVorticesPerPanel); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_ve_y = NULL;
	err = cudaMalloc((void **)&d_ve_y, sizeSubjectPanelsFloat*noOfVorticesPerPanel); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_ve_z = NULL;
	err = cudaMalloc((void **)&d_ve_z, sizeSubjectPanelsFloat*noOfVorticesPerPanel); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	// The memory for the object point velocities.
	float *d_cp_vx = NULL;
	err = cudaMalloc((void **)&d_cp_vx, sizeVelocityPredictionsFloat); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_cp_vy = NULL;
	err = cudaMalloc((void **)&d_cp_vy, sizeVelocityPredictionsFloat); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	float *d_cp_vz = NULL;
	err = cudaMalloc((void **)&d_cp_vz, sizeVelocityPredictionsFloat); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	// The memory for the panel vorticities
	float *d_vorticities = NULL;
	err = cudaMalloc((void **)&d_vorticities, sizeSubjectPanelsFloat); if (err != cudaSuccess) { exit(EXIT_FAILURE); }


	// ============================================================================
	// Copy host memory to device memory.
	// ============================================================================
	err = cudaMemcpy(d_cp_x, h_cp_x, sizeVelocityPredictionsFloat, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_cp_y, h_cp_y, sizeVelocityPredictionsFloat, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_cp_z, h_cp_z, sizeVelocityPredictionsFloat, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_vs_x, h_vs_x, sizeSubjectPanelsFloat*noOfVorticesPerPanel, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_vs_y, h_vs_y, sizeSubjectPanelsFloat*noOfVorticesPerPanel, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_vs_z, h_vs_z, sizeSubjectPanelsFloat*noOfVorticesPerPanel, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_ve_x, h_ve_x, sizeSubjectPanelsFloat*noOfVorticesPerPanel, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_ve_y, h_ve_y, sizeSubjectPanelsFloat*noOfVorticesPerPanel, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_ve_z, h_ve_z, sizeSubjectPanelsFloat*noOfVorticesPerPanel, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_cp_vx, h_cp_vx, sizeVelocityPredictionsFloat, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_cp_vy, h_cp_vy, sizeVelocityPredictionsFloat, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_cp_vz, h_cp_vz, sizeVelocityPredictionsFloat, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaMemcpy(d_vorticities, h_vorticities, sizeSubjectPanelsFloat, cudaMemcpyHostToDevice); if (err != cudaSuccess) { exit(EXIT_FAILURE); }

	// =========================================================================================
	// Call the Kernel.
	// =========================================================================================
	int noOfElementsInEachThread = 1; // The number of object point array elements computed by each instance of the kernel function.
	int totalNumberOfThreads = (noOfVelocityPredictions + noOfElementsInEachThread - 1) / noOfElementsInEachThread;

	// TODO: Temporarily reduce threadsPerBlock to 1 to see if it helps with cuPrint.

	int threadsPerBlock = 1; // 256; // When running out of resources, try reduce the threadsPerBlock.
	int totalNumberOfBlocks = (totalNumberOfThreads + threadsPerBlock - 1) / threadsPerBlock;
	int noOfBlocksX = 64;
	int noOfBlocksY = (totalNumberOfBlocks + noOfBlocksX - 1) / noOfBlocksX;

	dim3 grid(noOfBlocksX, noOfBlocksY, 1);
	dim3 block(threadsPerBlock, 1, 1);

	// Initialize tranche variables for tranche execution.
	int threadsPerTranche = 6000;
	int noOfTranches = (noOfVelocityPredictions + threadsPerTranche - 1) / threadsPerTranche;

	for (int trancheIndex = 0; trancheIndex<noOfTranches; trancheIndex++)
	{
		clock_t time_end;
		time_end = clock() + 10 * CLOCKS_PER_SEC / 1000;
		while (clock() < time_end)
		{
		}

		// Sort out tranche start index.
		int indexOfStartOfTranche = trancheIndex*threadsPerTranche;

		// Call noOfBlocksX*noOfBlocksY*threadsPerBlock instances of the kernel.
		kernelFunctionPredictVelocityAtPoint << <grid, block >> >(
			d_cp_x,
			d_cp_y,
			d_cp_z,
			d_vs_x,
			d_vs_y,
			d_vs_z,
			d_ve_x,
			d_ve_y,
			d_ve_z,
			d_cp_vx,
			d_cp_vy,
			d_cp_vz,
			d_vorticities,
			noOfVorticesPerPanel,
			noOfSubjectPanels,
			noOfElementsInEachThread, // Usually set to 1.
			noOfVelocityPredictions,
			threadsPerTranche,
			indexOfStartOfTranche,
			rankineAlgorithmIndex);

		cudaDeviceSynchronize();


		// Deal with any errors.
		err = cudaGetLastError();
		if (err != cudaSuccess)
		{
			std::cout << "Failed to launch kernelFunctionPredictVelocityAtPoint kernel (error code " << cudaGetErrorString(err) << ")" << std::endl;
			exit(EXIT_FAILURE);
		}


	} // End of for tranches.

	err = cudaMemcpy(h_cp_vx, d_cp_vx, sizeVelocityPredictionsFloat, cudaMemcpyDeviceToHost);
	err = cudaMemcpy(h_cp_vy, d_cp_vy, sizeVelocityPredictionsFloat, cudaMemcpyDeviceToHost);
	err = cudaMemcpy(h_cp_vz, d_cp_vz, sizeVelocityPredictionsFloat, cudaMemcpyDeviceToHost);

	// Free the GPU memory.
	err = cudaFree(d_cp_x); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_cp_y); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_cp_z); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_vs_x); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_vs_y); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_vs_z); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_ve_x); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_ve_y); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_ve_z); if (err != cudaSuccess) { exit(EXIT_FAILURE); }

	err = cudaFree(d_cp_vx); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_cp_vy); if (err != cudaSuccess) { exit(EXIT_FAILURE); }
	err = cudaFree(d_cp_vz); if (err != cudaSuccess) { exit(EXIT_FAILURE); }

	err = cudaFree(d_vorticities); if (err != cudaSuccess) { exit(EXIT_FAILURE); }

} // End of ComputeVelocitiesForBatchOfPointsCuda.