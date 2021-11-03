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

struct action {
    __host__ __device__ bool operator() (double x, double y) {
        return std::fabs(x) < std::fabs(y);
    }
};

__global__ void swap(double* a, int k, int y, int n) {
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
	int offsetx = blockDim.x * gridDim.x;
    double w;
    for(int i = idx; i < n+1; i += offsetx) {
        w = a[i * n + k];
        a[i * n + k] = a[i * n + y];
        a[i * n + y] = w;
    }
}

__global__ void change(double *data, int n, int m)
{
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    int offsetx = blockDim.x * gridDim.x;
    int idy = blockDim.y * blockIdx.y + threadIdx.y;
	int offsety = blockDim.y * gridDim.y;

    for (int j = idx+ m +1; j < n; j += offsetx) {
            double d = data[m*n+j] / data[m*n+m];
            for (int i = m + idy; i < n + 1; i+=offsety) { //для приписанной матрицы
                data[i*n+j] = data[i*n+j] - data[i*n+m] * d;
            }
     }
}

int main() {
    int n;
    std::cin >> n;
    double* a = (double*)malloc(sizeof(double) * n * (n+1));
    double* x = (double*)malloc(sizeof(double) * n);
    double p;
    //ввод
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cin >> p;
            a[j*n + i] = p;
        }
    }

    for (int i = 0; i < n; i++) {
            std::cin >> p;
            int s = n*n + i;
            a[s] = p;
    }


    //память куды
    double* data;
    CSC(cudaMalloc(&data, sizeof(double) * n * (n+1)));
    CSC(cudaMemcpy(data, a, sizeof(double) * n * (n+1), cudaMemcpyHostToDevice));

    action comparator;

    thrust::device_ptr<double> i_ptr, i_max_ptr;

    double* dop = (double*)malloc(sizeof(double));
    
    for(int m = 0; m < n-1; m++) {
        int i_max = m;
        i_ptr = thrust::device_pointer_cast(data + m * n);
        i_max_ptr = thrust::max_element(i_ptr + m, i_ptr + n, comparator);
        i_max = i_max_ptr - i_ptr;
        if(m != i_max) {
            swap<<<256, 256>>>(data, m, i_max, n);
        }
        CSC(cudaMemcpy(dop, data+m*n+m, sizeof(double), cudaMemcpyDeviceToHost));
        if (*dop != 0) {
            change<<<dim3(32, 32), dim3(32, 32)>>>(data, n, m);
        }
    }

    CSC(cudaMemcpy(a, data, sizeof(double) * n * (n+1), cudaMemcpyDeviceToHost));
    
    x[n - 1] = a[(n+1)*n - 1] / a[(n+1)*n - n - 1];

    for(int k = n-1; k >= 0; k--) {
        double d = 0;
        for (int j = k + 1; j < n; j++) {
            d = d + a[j*n + k] * x[j];
        }
        x[k] = (a[n*n + k] - d) / a[k*n + k];
    }
    std::cout.precision(10);
    std::cout.setf(std::ios::scientific);

    for (int i = 0; i < n; i++) {
          std::cout << x[i] << " ";

    }
    CSC(cudaFree(data));
    free(a);
    free(x);
    return 0;
}