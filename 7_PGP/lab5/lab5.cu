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

__global__ void gpu_add_block_sums(int* out, int* const in, int* block_sums)
{ 
    int d_block_sum_val = block_sums[blockIdx.x];
    int cpy_idx = 2 * blockIdx.x * blockDim.x + threadIdx.x;
    if (cpy_idx < 257)
    {
        out[cpy_idx] = in[cpy_idx] + d_block_sum_val;
        if (cpy_idx + blockDim.x < 257)
            out[cpy_idx + blockDim.x] = in[cpy_idx + blockDim.x] + d_block_sum_val;
    }
}

__global__ void prescan(int* d_out, int* d_in, int* block_sums, int shmem_size, int blocks, int max_block_size) {
    __shared__ int temp[257];

    int idx = threadIdx.x;
    int ai = idx;
    int bi = idx + blockDim.x;

    temp[idx] = 0;
    temp[idx + blockDim.x] = 0;
    temp[idx + blockDim.x + (blockDim.x >> what_pow(blocks))] = 0;
    
    __syncthreads();

    int cpy_idx = max_block_size * blockIdx.x + threadIdx.x;
    if (cpy_idx < 257)
    {
        temp[ai + no_conflict_offset(ai, blocks)] = d_in[cpy_idx];
        if (cpy_idx + blockDim.x < 257)
            temp[bi + no_conflict_offset(bi, blocks)] = d_in[cpy_idx + blockDim.x];
    }

    int offset = 1;
    for (int d = max_block_size >> 1; d > 0; d >>= 1)
    {
        __syncthreads();

        if (idx < d)
        {
            int ai = offset * ((idx << 1) + 1) - 1;
            int bi = offset * ((idx << 1) + 2) - 1;
            ai += no_conflict_offset(ai, blocks);
            bi += no_conflict_offset(bi, blocks);

            temp[bi] += temp[ai];
        }
        offset <<= 1;
    }
    
    if (idx == 0) 
    { 
        block_sums[blockIdx.x] = temp[max_block_size - 1 + no_conflict_offset(max_block_size - 1, blocks)];
        temp[max_block_size - 1 + no_conflict_offset(max_block_size - 1, blocks)] = 0;
    }
    for (int d = 1; d < max_block_size; d <<= 1)
    {
        offset >>= 1;
        __syncthreads();

        if (idx < d)
        {
            int ai = offset * ((idx << 1) + 1) - 1;
            int bi = offset * ((idx << 1) + 2) - 1;
            ai += no_conflict_offset(ai, blocks);
            bi += no_conflict_offset(bi, blocks);

            unsigned int t = temp[ai];
            temp[ai] = temp[bi];
            temp[bi] += t;
        }
    }
    __syncthreads();
    if (cpy_idx < 257)
    {
        d_out[cpy_idx] = temp[ai + no_conflict_offset(ai, blocks)];
        if (cpy_idx + blockDim.x < 257)
            d_out[cpy_idx + blockDim.x] = temp[bi + no_conflict_offset(bi, blocks)];
    }
}
 
void scan(int* counts, int* out, int blocks, int max_block_size) {
    CSC(cudaMemset(out, 0, 257 * sizeof(int)));
    int block_size = max_block_size / 2;
    int grid_size = 257 / max_block_size;
    
    if (257 % max_block_size != 0) {
        grid_size += 1;
    }
    int shmem_size = max_block_size + ((max_block_size) >> what_pow_h(blocks)); //к примеру 5
    int* block_sums;
    CSC(cudaMalloc(&block_sums, sizeof(int) * grid_size));
    CSC(cudaMemset(block_sums, 0, sizeof(int) * grid_size));

    prescan<<<grid_size, block_size, sizeof(int) * shmem_size>>>(out, counts, block_sums, shmem_size, blocks, max_block_size);

    if (grid_size <= max_block_size)
    {
        int* d_dummy_blocks_sums;
        CSC(cudaMalloc(&d_dummy_blocks_sums, sizeof(int)));
        CSC(cudaMemset(d_dummy_blocks_sums, 0, sizeof(int)));

        prescan<<<1, block_size, sizeof(int) * shmem_size>>>(block_sums, block_sums, d_dummy_blocks_sums, grid_size, shmem_size, max_block_size);
        
        CSC(cudaFree(d_dummy_blocks_sums));
    }
    else
    {
        int* d_in_block_sums;
        CSC(cudaMalloc(&d_in_block_sums, sizeof(int) * grid_size));
        CSC(cudaMemcpy(d_in_block_sums, block_sums, sizeof(int) * grid_size, cudaMemcpyDeviceToDevice));

        scan(block_sums, d_in_block_sums, blocks, grid_size);

        CSC(cudaFree(d_in_block_sums));
    }
    gpu_add_block_sums<<<grid_size, block_size>>>(out, out, block_sums);
    CSC(cudaFree(block_sums));
}

__global__ void kernel(int* pref, unsigned char* out, int n, unsigned char* array)
{
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    int offsetx = blockDim.x * gridDim.x;

    for(int i = n-1-idx; i >= 0; i -= offsetx){
        out[atomicAdd(pref + array[i]+1, -1)-1] = array[i];
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
    for(int i = 0; i < 257; i ++) {
        atomicAdd(out + i, *(temp + i));
    }
}

int main() {
    int n;
    fread(&n, sizeof(int), 1, stdin);
    unsigned char* array = (unsigned char*)malloc(sizeof(unsigned char) * n); //uchar
    fread(array, sizeof(unsigned char), n, stdin);
    unsigned char* gpu_array;
    CSC(cudaMalloc(&gpu_array, sizeof(unsigned char) * n));
    CSC(cudaMemcpy(gpu_array, array, sizeof(unsigned char) * n, cudaMemcpyHostToDevice));
    
    int* counts = (int*)malloc(sizeof(int) * 257);
    for (int i = 0; i < 257; i++) {
        counts[i] = 0;
    }
    int* gpu_counts;
    CSC(cudaMalloc(&gpu_counts, sizeof(int) * 257));
    CSC(cudaMemcpy(gpu_counts, counts, sizeof(int) * 257, cudaMemcpyHostToDevice));
    
    hist<<<32,32>>>(gpu_array, n, gpu_counts);
    CSC(cudaMemcpy(counts, gpu_counts, sizeof(int) * 257, cudaMemcpyDeviceToHost));

    int* gpu_pref;
    CSC(cudaMalloc(&gpu_pref, sizeof(int) * 257));
    scan(gpu_counts, gpu_pref, 32, 32);
    
    unsigned char* gpu_out;
    CSC(cudaMalloc(&gpu_out, sizeof(unsigned char) * n));
    kernel<<<32,32>>>(gpu_pref, gpu_out, n, gpu_array);
    CSC(cudaMemcpy(array, gpu_out, sizeof(unsigned char) * n, cudaMemcpyDeviceToHost));
    fwrite(array, sizeof(unsigned char), n, stdout);
    return 0;
}