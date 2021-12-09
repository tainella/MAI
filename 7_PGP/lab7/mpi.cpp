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

#define _i(i, j, j_size) ((j + 1) * (j_size+2) + i + 1)

#define _ib(i, j, b_size) (j * b_size + i)

int main(int argc, char *argv[]) {
    //входные данные
    int shape[3];
    int block_size[3];
    std::string out_name;
    double eps;
    double l_size[3];
    double u[6];
    double u0;

    //данные для работы
    int id, numproc;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    fprintf(stderr, "%d(%d)\n", id, numproc);

    MPI_Barrier(MPI_COMM_WORLD); //синхронизация

    if (id == 0) { //считывает первый процесс
        std::cin >> shape[0] >> shape[1] >> shape[2];
        std::cin >> block_size[0] >> block_size[1] >> block_size[2];
        std::cin >> out_name;
        std::cin >> eps;
        std::cin >> l_size[0] >> l_size[1] >> l_size[2];
        for (int i = 0; i < 6; i++) {
            std::cin >> u[i];
        }
        std::cin >> u0;
    }

    //пересылаем всем другим процессам данные
    MPI_Bcast(shape, 3, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(block_size, 3, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&eps, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(l_size, 3, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(u, 6, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&u0, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double hx, hy, hz;
    hx = l[0] / ((double)shape[0] * block_size[0]);
    hy = l[1] / ((double)shape[1] * block_size[1]);
    hz = l[2] / ((double)shape[2] * block_size[2]);

    double* data[3];
    double* next[3];
    data[0] = (double*)malloc(sizeof(double) * (shape[1] + 2) * (shape[2] + 2)); // y z
    next[0] = (double*)malloc(sizeof(double) * (shape[1] + 2) * (shape[2] + 2));
    data[1] = (double*)malloc(sizeof(double) * (shape[0] + 2) * (shape[2] + 2)); // x z
    next[1] = (double*)malloc(sizeof(double) * (shape[0] + 2) * (shape[2] + 2));
    data[2] = (double*)malloc(sizeof(double) * (shape[0] + 2) * (shape[1] + 2)); // x y
    next[2] = (double*)malloc(sizeof(double) * (shape[0] + 2) * (shape[1] + 2));

    double* buf[2];
    buf[0] = (double*)malloc(sizeof(double) * (shape[0] + 2) * (shape[1] + 2) * (shape[2] + 2)); //x y z
    buf[1] = (double*)malloc(sizeof(double) * (shape[0] + 2) * (shape[1] + 2) * (shape[2] + 2));

    //???
    int buf_size;
    MPI_Pack_size((shape[0] + 2) * (shape[1] + 2) * (shape[2] + 2), MPI_DOUBLE, MPI_COMM_WORLD, &buf_size);
    buf_size = 4 * (buf_size + MPI_BSEND_OVERHEAD); //?
    double* buffer = (double*)malloc(buf_size);
    MPI_Buffer_attach(buffer, buf_size);

    //инициализация
    for (int i = 0; i < shape[1]; i++){
        for (int j = 0; j < shape[2]; j++){
            data[0][_i(i, j, shape[2])] = u0;
            //next[0][_i(i, j, shape[2])] = u0;
        }
    }
    for (int i = 0; i < shape[0]; i++){
        for (int j = 0; j < shape[2]; j++){
            data[1][_i(i, j, shape[2])] = u0;
            //next[0][_i(i, j, shape[2])] = u0;
        }
    }
    for (int i = 0; i < shape[0]; i++){
        for (int j = 0; j < shape[1]; j++){
            data[2][_i(i, j, shape[1])] = u0;
            //next[0][_i(i, j, shape[2])] = u0;
        }
    }
    

    MPI_Finalize();

    for (int i = 0; i < 3; i++) {
        free(data[i]);
    }

    return 0;
}
