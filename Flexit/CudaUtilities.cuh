#pragma once
#include <vector> 
#include <cuda_runtime.h> 

int GetDeviceCount();
std::string InitializeDevice(int argc, char *argv[], std::string *gpuString);
void InitializeData(std::vector<float>& data);
__global__ __device__ void cubeKernel(float* result, float* data);
void RunCubeKernel(std::vector<float>& data, std::vector<float>& result);