#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <mpi.h>

#define CSC(call)  \
do { \
	cudaError_t state = call; \
	if (state != cudaSuccess) { \
		fprintf(stderr, "ERROR: %s:%d. Message: %s\n", __FILE__,__LINE__,cudaGetErrorString(state)); \
		exit(0); \
	} \
} while (0); \

int main(int argc, char *argv[]) {
	int shape[3];
	std::cin >> shape[0] >> shape[1] >> shape[2];

	int block_size[3];
	std::cin >> block_size[0] >> block_size[1] >> block_size[2];

	std::string out_name;
	std::cin >> out_name;

	double eps;
	std::cin >> eps;

	double l_size[3];
	std::cin >> l_size[0] >> l_size[1] >> l_size[2];

	double u[6];
	for (int i = 0; i < 6; i++) {
		std::cin >> u[i];
	}
	double u0;
	std::cin >> u0;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	fprintf(stderr, "%d(%d)\n", rank, nprocs);

/*	for(i = 0; i < n; i++)
		data[i] = rank * 16 + i; */

	MPI_File fp;
	MPI_Datatype arraytype;

	MPI_Type_contiguous(n, MPI_INT, &arraytype);
	MPI_Type_commit(&arraytype);

//	MPI_File_delete("data", MPI_INFO_NULL);
//	MPI_File_open(MPI_COMM_WORLD, "data", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fp);
	MPI_File_open(MPI_COMM_WORLD, "data", MPI_MODE_RDONLY, MPI_INFO_NULL, &fp);
	MPI_File_set_view(fp, rank * n * sizeof(int), MPI_INT, arraytype, "native", MPI_INFO_NULL);

//	MPI_File_write(fp, data, n, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_read(fp, data, n, MPI_INT, MPI_STATUS_IGNORE);

	printf("%d: ", rank);
	for(i = 0; i < n; i++)
		printf("%d ", data[i]);
	printf("\n");

	MPI_File_close(&fp);

	MPI_Finalize();
	free(data);

	return 0;
}
