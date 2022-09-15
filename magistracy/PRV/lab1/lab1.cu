#include <stdio.h>
#include <stdlib.h>

#define CSC(call)  \
do { \
	cudaError_t state = call; \
	if (state != cudaSuccess) { \
		fprintf(stderr, "ERROR: %s:%d. Message: %s\n", __FILE__,__LINE__,cudaGetErrorString(state)); \
		exit(0); \
	} \
} while (0); \

__global__ void kernel(int* v1, long long n, int a0, int d) {
	long long i = blockDim.x * blockIdx.x + threadIdx.x;
	long long offset = blockDim.x * gridDim.x;

	while (i < n) {
		v1[i] = a0 + d * i;
		i = i + offset;
	}
}

int main() {
	long long n = 1500000000;
	//scanf("%lld", &n);

    int a0 = 1;
    //scanf("%d", &a0);

    int d = 1;
    //scanf("%d", &d);
    //for (n = 4; n <= 256; n = n * 2) {
        int* v1 = (int*)malloc(n * sizeof(int)); //элементы прогрессии 

        int* dev_v1;
        CSC(cudaMalloc(&dev_v1, sizeof(int) * n));
        cudaEvent_t start, end;
        CSC(cudaEventCreate(&start));
        CSC(cudaEventCreate(&end));
        CSC(cudaEventRecord(start));

        kernel<<<256,256>>>(dev_v1, n, a0, d);

        CSC(cudaEventRecord(end));
        CSC(cudaEventSynchronize(end));
        float t;
        CSC(cudaEventElapsedTime(&t, start, end));
        CSC(cudaEventDestroy(start));
        CSC(cudaEventDestroy(end));
        printf("time: %.10lf\n", t);

        CSC(cudaMemcpy(v1, dev_v1, sizeof(int) * n, cudaMemcpyDeviceToHost));
        CSC(cudaFree(dev_v1));

        for (long long i = 0; i < n; i++) {
            printf("%d ", v1[i]);
        }
        printf("\n\n");
        free(v1);
        printf("time: %.10lf\n", t);
    //}
	return 0;
}