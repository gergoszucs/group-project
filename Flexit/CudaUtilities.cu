#include <iostream> 
#include <vector> 
#include <cuda_runtime.h> 
#include <sstream>

#pragma comment(lib, "cudart") 

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::vector;

static const int MaxSize = 96;

// CUDA kernel: cubes each array value 
__global__ void cubeKernel(float* result, float* data)
{
	int idx = threadIdx.x;
	float f = data[idx];
	result[idx] = f * f * f;
}

// Initializes data on the host 
void InitializeData(vector<float>& data)
{
	for (int i = 0; i < MaxSize; ++i)
	{
		data[i] = static_cast<float>(i);
	}
}

// Executes CUDA kernel 
void RunCubeKernel(vector<float>& data, vector<float>& result)
{
	const size_t size = MaxSize * sizeof(float);

	// TODO: test for error 
	float* d;
	float* r;
	cudaError hr;

	hr = cudaMalloc(reinterpret_cast<void**>(&d), size);            // Could return 46 if device is unavailable. 
	if (hr == cudaErrorDevicesUnavailable)
	{
		cerr << "Close all browsers and rerun" << endl;
		throw std::runtime_error("Close all browsers and rerun");
	}

	hr = cudaMalloc(reinterpret_cast<void**>(&r), size);
	if (hr == cudaErrorDevicesUnavailable)
	{
		cerr << "Close all browsers and rerun" << endl;
		throw std::runtime_error("Close all browsers and rerun");
	}

	// Copy data to the device 
	cudaMemcpy(d, &data[0], size, cudaMemcpyHostToDevice);

	// Launch kernel: 1 block, 96 threads 
	// Important: Do not exceed number of threads returned by the device query, 1024 on my computer. 
	cubeKernel << <1, MaxSize >> > (r, d);

	// Copy back to the host 
	cudaMemcpy(&result[0], r, size, cudaMemcpyDeviceToHost);

	// Free device memory 
	cudaFree(d);
	cudaFree(r);
}

int GetDeviceCount()
{
	int devCount;
	cudaGetDeviceCount(&devCount);
	return devCount;
}

std::string InitializeDevice(int argc, char *argv[], std::string *gpuString)
{
	std::string outputString;

	int devCount;
	cudaGetDeviceCount(&devCount);

	for (int i = 0; i < devCount; ++i)
	{
		cudaDeviceProp devProp;
		cudaGetDeviceProperties(&devProp, i);

		gpuString->append(devProp.name);

		std::ostringstream s1;
		s1 << "Device index: " << i << std::endl;
		outputString.append(s1.str());

		std::ostringstream s2;
		s2 << "Major revision number: " << devProp.major << std::endl;
		outputString.append(s2.str());

		std::ostringstream s3;
		s3 << "Minor revision number: " << devProp.minor << std::endl;
		outputString.append(s3.str());

		std::ostringstream s4;
		s4 << "Name: " << devProp.name << std::endl;
		outputString.append(s4.str());

		std::ostringstream s5;
		s5 << "Total global memory: " << devProp.totalGlobalMem << std::endl;
		outputString.append(s5.str());

		std::ostringstream s6;
		s6 << "Total shared memory per block: " << devProp.sharedMemPerBlock << std::endl;
		outputString.append(s6.str());

		std::ostringstream s7;
		s7 << "Total registers per block: " << devProp.regsPerBlock << std::endl;
		outputString.append(s7.str());

		std::ostringstream s8;
		s8 << "Warp size: " << devProp.warpSize << " (should be 32)" << std::endl;
		outputString.append(s8.str());

		std::ostringstream s9;
		s9 << "Maximum memory pitch: " << devProp.memPitch << std::endl;
		outputString.append(s9.str());

		std::ostringstream s10;
		s10 << "Maximum threads per block: " << devProp.maxThreadsPerBlock << std::endl;
		outputString.append(s10.str());

		for (int j = 0; j < 3; ++j)
		{
			std::ostringstream s11;
			s11 << "Maximum dimension " << j << " of block: " << devProp.maxThreadsDim[j] << std::endl;
			outputString.append(s11.str());
		}

		for (int j = 0; j < 3; ++j)
		{
			std::ostringstream s12;
			s12 << "Maximum dimension " << j << " of grid: " << devProp.maxGridSize[j] << std::endl;
			outputString.append(s12.str());
		}

		std::ostringstream s13;
		s13 << "Clock rate: " << devProp.clockRate << std::endl;
		outputString.append(s13.str());

		std::ostringstream s14;
		s14 << "Total constant memory: " << devProp.totalConstMem << std::endl;
		outputString.append(s14.str());

		std::ostringstream s15;
		s15 << "Texture alignment: " << devProp.textureAlignment << std::endl;
		outputString.append(s15.str());

		std::ostringstream s16;
		s16 << "Concurrent copy and execution: " << (devProp.deviceOverlap ? "Yes" : "No") << std::endl;
		outputString.append(s16.str());

		std::ostringstream s17;
		s17 << "Number of multiprocessors: " << devProp.multiProcessorCount << std::endl;
		outputString.append(s17.str());

		std::ostringstream s18;
		s18 << "Kernel execution timeout: " << (devProp.kernelExecTimeoutEnabled ? "Yes" : "No") << std::endl << std::endl;
		outputString.append(s18.str());
	}

	return outputString;
}