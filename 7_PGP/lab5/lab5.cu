#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>

#include <thrust/extrema.h>
#include <thrust/device_vector.h>

#define CSC(call)  \
do { \
	cudaError_t state = call; \
	if (state != cudaSuccess) { \
		fprintf(stderr, "ERROR: %s:%d. Message: %s\n", __FILE__,__LINE__,cudaGetErrorString(state)); \
		exit(0); \
	} \
} while (0); \

__global__ void kernel()
{
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    int offsetx = blockDim.x * gridDim.x;
    int idy = blockDim.y * blockIdx.y + threadIdx.y;
	int offsety = blockDim.y * gridDim.y;
    
}

__global__ void hist(unsigned char* array, int n, int* out) {
    __shared__ int temp[256];
    
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    int offsetx = blockDim.x * gridDim.x;
}

int main() {
    int n;
    std::cin >> std::hex >> n;
    unsigned char* array = (unsigned char*)malloc(sizeof(unsigned char) * n); //uchar
    for(int i = 0; i < n; i++) {
        std::cin >> std::hex >> array[i];
    }
    double* gpu_array;
    CSC(cudaMalloc(&gpu_array, sizeof(unsigned char) * n));
    CSC(cudaMemcpy(gpu_array, array, sizeof(unsigned char) * n, cudaMemcpyHostToDevice));
    
    int* counts = (int*)malloc(sizeof(int) * 256);
    for (int i = 0; i < 256; i++) {
        counts[i] = 0;
    }
    double* gpu_counts;
    CSC(cudaMalloc(&gpu_counts, sizeof(int) * 256));
    CSC(cudaMemcpy(gpu_counts, counts, sizeof(int) * 256, cudaMemcpyHostToDevice));
    
    hist<<<32,32>>>(gpu_array, n, gpu_counts);
    

    
    for(int i = 0; i < n; i++) {
        std::cout<< std::hex << array[i] << " ";
    }
    return 0;
}
