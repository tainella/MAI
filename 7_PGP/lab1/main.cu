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

__global__ void kernel(double* v1, double* v2, double* v3, long long n) {
	long long i = blockDim.x * blockIdx.x + threadIdx.x;
	long long offset = blockDim.x * gridDim.x;

	while (i < n) {
		if (v1[i] < v2[i]) {
			v3[i] = v1[i];
		}
		else {
			v3[i] = v2[i];
		}
		i = i + offset;
	}
}

int main() {
	long long n;
	scanf("%lld", &n);

	double* v1 = (double*)malloc(n * sizeof(double));
	double* v2 = (double*)malloc(n * sizeof(double));

	for(long long i = 0; i < n; i++) {
		scanf("%lf", &v1[i]);
	}
	for(long long i = 0; i < n; i++) {
			scanf("%lf", &v2[i]);
	}

	double* dev_v1;
	double* dev_v2;
	double* dev_v3;
	CSC(cudaMalloc(&dev_v1, sizeof(double) * n));
	CSC(cudaMalloc(&dev_v2, sizeof(double) * n));
	CSC(cudaMalloc(&dev_v3, sizeof(double) * n));

	CSC(cudaMemcpy(dev_v1, v1, sizeof(double) * n, cudaMemcpyHostToDevice));
	CSC(cudaMemcpy(dev_v2, v2, sizeof(double) * n, cudaMemcpyHostToDevice));

	kernel<<<256,256>>>(dev_v1, dev_v2, dev_v3, n);

	CSC(cudaMemcpy(v1, dev_v3, sizeof(double) * n, cudaMemcpyDeviceToHost));
	CSC(cudaFree(dev_v1));
	CSC(cudaFree(dev_v2));
	CSC(cudaFree(dev_v3));

	for (long long i = 0; i < n; i++) {
		printf("%.10lf ", v1[i]);
	}
	printf("\n");
	free(v1);
	free(v2);
	return 0;
}
