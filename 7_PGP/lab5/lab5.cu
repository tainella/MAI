#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>

#include <thrust/extrema.h>
#include <thrust/device_vector.h>

#define THREAD_NUM 1024

#define CSC(call)  \
do { \
	cudaError_t state = call; \
	if (state != cudaSuccess) { \
		fprintf(stderr, "ERROR: %s:%d. Message: %s\n", __FILE__,__LINE__,cudaGetErrorString(state)); \
		exit(0); \
	} \
} while (0); \

__device__ int what_pow(int n) {
    int k = 0;
    while (n > 1) {
        n = n / 2;
        k++;
    }
    return k;
}

__host__ int what_pow_h(int n) {
    int k = 0;
    while (n > 1) {
        n = n / 2;
        k++;
    }
    return k;
}

__device__  int no_conflict_offset(int n, int banks) {
    return ((n) >> banks + (n) >> (2 * what_pow(banks)));
}

__global__ void prescan(int* d_out, const int* d_in, int blocks) {
    int sum;
    int max_block_size = 256; //128
    __shared__ int temp[256];

    int idx = threadIdx.x;
    int ai = idx;

    temp[idx] = 0;
    temp[idx + blockDim.x] = 0;
    
    __syncthreads();

    temp[ai] = d_in[ai];
    temp[128 + ai] = d_in[128 + ai];
    
    int offset = 1;
    for (int d = max_block_size >> 1; d > 0; d >>= 1)
    {
        __syncthreads();

        if (idx < d)
        {
            int ai = offset * ((idx << 1) + 1) - 1;
            int bi = offset * ((idx << 1) + 2) - 1;
            temp[bi] += temp[ai];
        }
        offset <<= 1;
    }
    if (idx == 0) { 
        sum = temp[255];
        //printf("sum:%d\n", sum);
        temp[255] = 0;
    }
    //обратный ход
    for (int d = 1; d < max_block_size; d <<= 1) {
        offset >>= 1;
        __syncthreads();

        if (idx < d)
        {
            int i1 = offset * ((idx << 1) + 1) - 1;
            int i2 = offset * ((idx << 1) + 2) - 1;

            int t = temp[i1];
            temp[i1] = temp[i2];
            temp[i2] += t;
        }
    }
    __syncthreads();
    if (idx == 0) { //переделать из исключающего в включающий
        for (int j = 1; j < 256; j++) {
            d_out[j - 1] = temp[j];
        }
         //printf("sum2: %d\n", sum);
         d_out[255] = sum;
         /*for (int j = 0; j < 256; j++) {
            printf("%d ", temp[j]);
         }
         printf("\n|||||||||\n");*/
    }
}

__global__ void kernel(int* pref, unsigned char* out){
    int idx = blockDim.x * blockIdx.x +  threadIdx.x;
    int step = blockDim.x * gridDim.x;

    for(int tid = idx; tid < 256; tid += step){
        int low = tid ? pref[tid-1] : 0;

        for(int i = pref[tid] - 1; i >= low; --i){
            out[i] = tid;
        }
    }
}

__global__ void hist(unsigned char* array, int n, int* out) {
    __shared__ int temp[257];
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    int offsetx = blockDim.x * gridDim.x;

    for(int i = idx; i < n; i += offsetx) {
        atomicAdd(temp + array[i], 1);
    }
    __syncthreads();
    if (idx == 0)
    for(int i = 0; i < 257; i++) {
        atomicAdd(out + i, *(temp + i));
    }
}

int main() {
    int n;
    //std::cin >> n;
    fread(&n, sizeof(int), 1, stdin);
    unsigned char* array = (unsigned char*)malloc(sizeof(unsigned char) * n); //uchar
    if (fread(array, sizeof(unsigned char), n, stdin) != n) {
        std::cout << "не считал\n";
    }
    
    /*for (int i = 0; i < n; i++) {
       // std::cin >> array[i];
       array[i] = 255;
    }
    for (int i = 0; i < 10 && i < n; i++) {
        //printf("%02X ", array[i]);
        std::cerr << array[i] << " ";
    }*/
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

    int* gpu_pref;
    CSC(cudaMalloc(&gpu_pref, sizeof(int) * 256));
    CSC(cudaMemset(gpu_pref, 0, sizeof(int) * 256));

    hist<<<32,32>>>(gpu_array, n, gpu_counts);
    CSC(cudaMemcpy(counts, gpu_counts, sizeof(int) * 256, cudaMemcpyDeviceToHost));
    /*for (int i = 0; i < 256; i++) {
        std::cout << counts[i] << " ";
    }
    std::cout << "\n|\n";
    */
    int pref[256];

    prescan<<<1,128>>>(gpu_pref, gpu_counts, 1);
    CSC(cudaMemcpy(pref, gpu_pref, sizeof(int) * 256, cudaMemcpyDeviceToHost));
    /*std::cout << "pref:\n";
    for (int i = 0; i < 256; i++) {
        std::cout << pref[i] << " ";
    }
*/
    unsigned char* gpu_out;
    CSC(cudaMalloc(&gpu_out, sizeof(unsigned char) * n));
    CSC(cudaMemset(gpu_out, 0, sizeof(unsigned char) * n));

    kernel<<<32,32>>>(gpu_pref, gpu_out);

    //std::cout << "\n|\n";
    
    CSC(cudaMemcpy(array, gpu_out, sizeof(unsigned char) * n, cudaMemcpyDeviceToHost));

    /*std::cerr << "out:\n";
    for (int i = 0; i < 10 && i < n; i++) {
        //printf("%02X ", array[i]);
        std::cerr << array[i] << "| ";
    }*/
    fwrite(array, sizeof(unsigned char), n, stdout);

    /*for (int i = n - 200; i < n; i++) {
        printf("%02X ", array[i]);
    }*/
    return 0;
}