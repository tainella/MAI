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

//по порядку ввода
#define down 0
#define up 1
#define left 2
#define right 3
#define front 4
#define back 5

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
    int data_sizes[3];
    double* next[3];
    data[0] = (double*)malloc(sizeof(double) * (shape[1] + 2) * (shape[2] + 2)); // y z
    next[0] = (double*)malloc(sizeof(double) * (shape[1] + 2) * (shape[2] + 2));
    data_sizes[0] = (shape[1] + 2) * (shape[2] + 2);

    data[1] = (double*)malloc(sizeof(double) * (shape[0] + 2) * (shape[2] + 2)); // x z
    next[1] = (double*)malloc(sizeof(double) * (shape[0] + 2) * (shape[2] + 2));
    data_sizes[1] = (shape[0] + 2) * (shape[2] + 2);

    data[2] = (double*)malloc(sizeof(double) * (shape[0] + 2) * (shape[1] + 2)); // x y
    next[2] = (double*)malloc(sizeof(double) * (shape[0] + 2) * (shape[1] + 2));
    data_sizes[2] = (shape[0] + 2) * (shape[1] + 2);

    double* buf[2];
    buf[0] = (double*)malloc(sizeof(double) * (shape[0] + 2) * (shape[1] + 2) * (shape[2] + 2)); //x y z
    buf[1] = (double*)malloc(sizeof(double) * (shape[0] + 2) * (shape[1] + 2) * (shape[2] + 2));
    int bs = (shape[0] + 2) * (shape[1] + 2) * (shape[2] + 2);
    for (int i = 0; i < bs; i++) {
        buf[0][i] = u0;
    }

    //инициализация
    for (int i = 0; i < shape[1]; i++){
        for (int j = 0; j < shape[2]; j++){
            next[0][_i(i, j, shape[2])] = u0; //y z
        }
    }

    for (int i = 0; i < shape[0]; i++){
        for (int j = 0; j < shape[2]; j++){
            //data[1][_i(i, j, shape[2])] = u0; // x z
            next[1][_i(i, j, shape[2])] = u0;
        }
    }
    for (int i = 0; i < shape[0]; i++){
        for (int j = 0; j < shape[1]; j++){
            //data[2][_i(i, j, shape[1])] = u0; // x y
            next[2][_i(i, j, shape[1])] = u0;
        }
    }
    //крайние точки
    for(int dir = 0; dir < 3; ++dir){
        int dd= dir << 1;
        if (coord[dir] == 0) {
            fill_n(data[dd], data_sizes[dir], u[dd]);
        }
        else if (coord[dir] == shape[dir] - 1) {
            fill_n(data[dd+1], data_sizes[dir], u[dd+1]);
        }
    }


    int period[3], coords[3]; // координаты
    int neighb_ranks[6];
    period[0] = 0;
    period[1] = 0;
    period[2] = 0;
    MPI_Comm grid_comm; // коммуникатор

    //топология
    MPI_Cart_create(MPI_COMM_WORLD, 3, shape, period, false, &grid_comm); //новый коммуникатор для топологии
    MPI_Comm_rank(grid_comm, &id); //получение нового ранга
    MPI_Comm_size(grid_comm, &numproc); //получить число процессов
    MPI_Cart_coords(grid_comm, id, 3, coords); //получить координаты

    //ранги соседей
    MPI_Cart_shift(grid_comm, dir_x, 1, &neighb_ranks[left], &neighb_ranks[right]);
    MPI_Cart_shift(grid_comm, dir_y, 1, &neighb_ranks[front], &neighb_ranks[back]);
    MPI_Cart_shift(grid_comm, dir_z, 1, &neighb_ranks[down], &neighb_ranks[up]);

    double max_diff = 1; // maximum error
    while (max_diff >= eps) {
        MPI_Barrier(MPI_COMM_WORLD); //синхронизация процессов

        //
        
    }

    MPI_Finalize();

    for (int i = 0; i < 3; i++) {
        free(data[i]);
    }

    return 0;
}
