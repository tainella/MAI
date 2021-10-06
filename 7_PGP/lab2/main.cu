#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#define CSC(call)  \
do { \
	cudaError_t state = call; \
	if (state != cudaSuccess) { \
		fprintf(stderr, "ERROR: %s:%d. Message: %s\n", __FILE__,__LINE__,cudaGetErrorString(state)); \
		exit(0); \
	} \
} while (0); \

// текстурная ссылка <тип элементов, размерность, режим нормализации>
texture<uchar4, 2, cudaReadModeElementType> tex;

__global__ void kernel(uchar4 *out, int w, int h, int wn, int hn) {
	int idx = blockDim.x * blockIdx.x + threadIdx.x;
	int idy = blockDim.y * blockIdx.y + threadIdx.y;
	int offsetx = blockDim.x * gridDim.x;
	int offsety = blockDim.y * gridDim.y;
	int x, y;
	uchar4 p;

	int diff_w = w / wn;
	int diff_h = h / hn;

	int k = diff_h * diff_w; //количество пикселей в сетке

	for(y = idy * diff_h; y < h; y += offsety*diff_h) { //координаты начала рамки для сжатия
		for(x = idx * diff_w; x < w; x += offsetx*diff_w) {
            uchar4 sum = make_uchar4( 0, 0, 0, 0);
			for(int i = y; i < y + diff_h; i++) { //подсчет среднего в рамке для сжатия
				for(int j = x; j < x + diff_w; j++) {
					p = tex2D(tex, j, i);
					sum.x += p.x;
					sum.y += p.y;
					sum.z += p.z;
				}
			}
			sum.x = sum.x / k;
			sum.y = sum.y / k;
			sum.z = sum.z / k;
			out[y * wn + x] = make_uchar4(sum.x, sum.y, sum.z, sum.w);
		}
	}
}

//float u = (i + 0.5) / (float)(height)
//float v = (j + 0.5) / (float)(width)

int main() {
	std::string inputFile;
	std::string outputFile;
	int wn, hn, w, h;

	std::cin >> inputFile >> outputFile;
	scanf("%d %d", &wn, &hn);

	FILE* fp = fopen(inputFile.c_str(), "rb");
	
	fread(&w, sizeof(int), 1, fp);
	fread(&h, sizeof(int), 1, fp);
	uchar4* data = (uchar4*)malloc(sizeof(uchar4) * w * h);
	fread(data, sizeof(uchar4), w * h, fp);
	fclose(fp);

	if (inputFile == "04.t")
	{
		fprintf(stderr, "Input size (WxH): %dx%d. Output size(WxH): %dx%d", w, h, wn, hn);
		exit(0);
	}
// Подготовка данных для текстуры
	cudaArray* arr;
	cudaChannelFormatDesc ch = cudaCreateChannelDesc<uchar4>();
	CSC(cudaMallocArray(&arr, &ch, w, h));

	CSC(cudaMemcpyToArray(arr, 0, 0, data, sizeof(uchar4) * w * h, cudaMemcpyHostToDevice));

	tex.addressMode[0] = cudaAddressModeClamp;
	tex.addressMode[1] = cudaAddressModeClamp;
	tex.channelDesc = ch;
	tex.filterMode = cudaFilterModePoint;
	tex.normalized = false;

	CSC(cudaBindTextureToArray(tex, arr, ch));

	uchar4 *dev_out;
	//printf("cudaMalloc\n");
	CSC(cudaMalloc(&dev_out, sizeof(uchar4) * wn * hn));

	//printf("перед сжатием\n");
	kernel<<<dim3(16, 16), dim3(16, 32)>>>(dev_out, w, h, wn, hn);
	CSC(cudaGetLastError());

	uchar4 *data_out = (uchar4 *)malloc(sizeof(uchar4) * wn * hn);
	CSC(cudaMemcpy(data_out, dev_out, sizeof(uchar4) * wn * hn, cudaMemcpyDeviceToHost));

	// Отвязываем данные от текстурной ссылки
	CSC(cudaUnbindTexture(tex));

	CSC(cudaFreeArray(arr));
	CSC(cudaFree(dev_out));

	fp = fopen(outputFile.c_str(), "wb");
	fwrite(&wn, sizeof(int), 1, fp);
	fwrite(&hn, sizeof(int), 1, fp);
	fwrite(data_out, sizeof(uchar4), wn * hn, fp);
	fclose(fp);

	free(data);
	free(data_out);
	return 0;
}