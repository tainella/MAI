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


__global__ void scan(int* counts, int* out)
{
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    int offsetx = blockDim.x * gridDim.x;
    
    __shared__ unsigned int temp[];
    
    
    __syncthreads();
    
}

__global__ void kernel(int* pref, unsigned char* out, int n, unsigned char* array)
{
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    int offsetx = blockDim.x * gridDim.x;
    
    for(int i = idx; i < n; i += offsetx) {
        out[atomicAdd(pref + array[i], -1)] = array[i];
    }
    
}

__global__ void hist(unsigned char* array, int n, int* out) {
    __shared__ int temp[256];
    
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    int offsetx = blockDim.x * gridDim.x;

    for(int i = idx; i < n; i += offsetx) {
        atomicAdd(temp + array[i], 1);
    }
    __syncthreads();
    if (idx == 0)
    for(int i = 0; i < 256; i ++) {
        atomicAdd(out + i, *(temp + i));
    }
}

int main() {
    int n;
    //std::cin >> std::hex >> n;
    std::cin >> n;
    unsigned char* array = (unsigned char*)malloc(sizeof(unsigned char) * n); //uchar
    for(int i = 0; i < n; i++) {
        //std::cin >> std::hex >> array[i];
        std::cin >> array[i];
    }
    unsigned char* gpu_array;
    CSC(cudaMalloc(&gpu_array, sizeof(unsigned char) * n));
    CSC(cudaMemcpy(gpu_array, array, sizeof(unsigned char) * n, cudaMemcpyHostToDevice));
    
    int* counts = (int*)malloc(sizeof(int) * 256);
    for (int i = 0; i < 256; i++) {
        counts[i] = 0;
    }
    int* gpu_counts;
    CSC(cudaMalloc(&gpu_counts, sizeof(int) * 256));
    CSC(cudaMemcpy(gpu_counts, counts, sizeof(int) * 256, cudaMemcpyHostToDevice));
    
    hist<<<32,32>>>(gpu_array, n, gpu_counts);
    CSC(cudaMemcpy(counts, gpu_counts, sizeof(int) * 256, cudaMemcpyDeviceToHost));
    std::cout << "\n";
    std::cout << "\n";
    for(int i = 0; i < 256; i++) {
        std::cout << counts[i] << " ";
    }
    std::cout << "\n";

    int* gpu_pref;
    CSC(cudaMalloc(&gpu_pref, sizeof(int) * 256));
    scan<<<32,32>>>(gpu_counts, gpu_pref);
    
    unsigned char* gpu_out;
    CSC(cudaMalloc(&gpu_out, sizeof(unsigned char) * n));
    kernel<<<32,32>>>(gpu_pref, gpu_out, n, gpu_array);
    CSC(cudaMemcpy(array, gpu_out, sizeof(unsigned char) * n, cudaMemcpyDeviceToHost));

    for(int i = 0; i < n; i++) {
        //std::cout<< std::hex << array[i] << " ";
        std::cout << array[i] << " ";
    }
    return 0;
}