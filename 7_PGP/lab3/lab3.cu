#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>

#define CSC(call)  \
do { \
	cudaError_t state = call; \
	if (state != cudaSuccess) { \
		fprintf(stderr, "ERROR: %s:%d. Message: %s\n", __FILE__,__LINE__,cudaGetErrorString(state)); \
		exit(0); \
	} \
} while (0); \

typedef struct pair {
    double x;
    double y;
} pair;

__host__ double dist(double x1, double y1, double z1, double x2, double y2, double z2) {
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2));
}

__constant__ pair u[];

__global__ void kernel(uchar4 *data, int w, int h, int n) {
	int idx = blockDim.x * blockIdx.x + threadIdx.x;
	int idy = blockDim.y * blockIdx.y + threadIdx.y;
	int offsetx = blockDim.x * gridDim.x;
	int offsety = blockDim.y * gridDim.y;
	int x, y;
	
    for (y = idy; y < h; y += offsety) {
        for(x = idx; k < w; x < w; x += offsetx) {
            double mas[n];
            double4 p = data[y * w + x]; //int4
            for (int j = 0; j < n; j++) {
                double4 c = data[u[j].y * w + u[j].x];
                mas[j] = dist(c.x, c.y, c.z, p.x, p.y, p.z); //заполняем массив расстояний            
            }
            //поиск минимума
            double MIN = mas[0];
            int min_i = 0;
            for (int j = 0; j < n; j++) {
                if (mas[j] < MIN) {
                    MIN = mas[j];
                    min_i = j; 
                }
            }
            p.w = min_i;
            data[y*w+x] = p;
        }
    }         
}

int main() {
	std::string inputFile;
	std::string outputFile;
	int n, x, y;

	std::cin >> inputFile >> outputFile;
	scanf("%d", &n);
    pair centre[n];
    pair centre_new[n];
    for(int i = 0; i < n; i++) {
        scanf("%lf %lf", &x, &y);
        centre[i].x = x;
        centre[i].y = y;
    }

	FILE* fp = fopen(inputFile.c_str(), "rb");
	
	fread(&w, sizeof(int), 1, fp);
	fread(&h, sizeof(int), 1, fp);
	uchar4* data = (uchar4*)malloc(sizeof(uchar4) * w * h);
	fread(data, sizeof(uchar4), w * h, fp);
	fclose(fp);

    uchar4 *dev_data;
    CSC(cudaMalloc(&dev_data, sizeof(uchar4) * h * w));
    CSC(cudaMemcpy(dev_data, data, sizeof(uchar4) * h * w, cudaMemcpyHostToDevice));

    bool flag = true;
    while(flag) {
        CSC(cudaMemcpyToSymbol(u, centre, sizeof(pair) * количество, 0, cudaMemcpyHostToDevice));
        kernel<<<dim3(16, 16), dim3(16, 16)>>>(dev_data, w, h, n); 
        CSC(cudaMemcpy(data, dev_data, sizeof(uchar4) * h * w, cudaMemcpyDeviceToHost));
       
        //обновление центра кластеров
        long long int k[n] = 0;
        long long int sum[n][2];
        for(int y = 0; y < n; y++) {
            sum[y][0] = 0;
            sum[y][1] = 0;
        }
        int4 t;
        for(int y = 0; y < h; y++) {
            for(int x = 0; x < w; x++) {
                t = data[y*w+x];
                k[t.w] += 1;
                sum[t.w][0] = y;
                sum[t.w][1] = x;
            }
        }
        for(int r = 0; r < n; r++) {
            centre_new[r].x = sum[r][1] / k[r];
            centre_new[r].y = sum[r][0] / k[r];
        }
        
        //условие прекращения
        for(int i = 0; i < n; i++) {
            if (centre[i].x == centre_new[i].x && centre[i].y == centre_new[i].y) {
                //проверяем дальше
            }
            else {
                flag = false; //были не одинаковые
            }    
        }
        if (flag == false) {
            flag = true; //продолжаем обработку
            for(int i = 0; i < n; i++) { 
                centre[i].x = centre_new[i].x;
                centre[i].y = centre_new[i].y;
            }
        }
        else {
            flag = false; //заканчиваем, выход из цикла
        }
    }

    fp = fopen(outputFile.c_str(), "wb");
	fwrite(&w, sizeof(int), 1, fp);
	fwrite(&h, sizeof(int), 1, fp);
	fwrite(data, sizeof(uchar4), w * h, fp);
	fclose(fp);

	free(data);
	free(data_out);
	return 0;
}

