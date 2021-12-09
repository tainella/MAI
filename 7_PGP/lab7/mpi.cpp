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

void tranfer(double** d_in, double** d_out, int* data_sizes, int* coords, int* shape, int* neighb_ranks, MPI_Comm comm){
    MPI_Request in[6], out[6]; // statuses of exchange beetwen processes
    //отправка
    for(int dir = 0; dir < 3; dir++){
        int dd= dir << 1;
        if(coords[dir]){
            MPI_Isend(d_out[dd], data_sizes[dir], MPI_DOUBLE, neighb_ranks[dd], 0, comm, &out[dd]);

            MPI_Irecv(d_in[dd], data_sizes[dir], MPI_DOUBLE, neighb_ranks[dd], 0, comm, &in[dd]);
        }
        if(coords[dir] < shape[dir] - 1){
            MPI_Isend(d_out[dd + 1], data_sizes[dir], MPI_DOUBLE, neighb_ranks[dd + 1], 0, comm, &out[dd + 1]);
            
            MPI_Irecv(d_in[dd + 1], data_sizes[dir], MPI_DOUBLE, neighb_ranks[dd + 1], 0, comm, &in[dd + 1]);
        }
    }

    // ожидание
    for(int dir = 0; dir < 3; dir++){
        int dd = dir << 1;
        if(coords[dir] > 0){
            MPI_Status temp;
            MPI_Wait(&in[dd], &temp);
            MPI_Wait(&out[dd], &temp);
        }
        if(coords[dir] < shape[dir] - 1){
            MPI_Status temp;
            MPI_Wait(&in[dd+1], &temp);
            MPI_Wait(&out[dd+1], &temp);
        }
    }
}

void import(double* buf, double** data, int* data_sizes, int* block_size){
    int start[3];
    int mu[3];
    int lm[3];
    int f[3];

    for(int i = 0; i < 3; i++) { // инициализация
        f[i] = block_size[i] + 2;
        lm[i] = block_size[i] + 1;
        mu[i] = 1;
        start[i] = 1;
    }

    for(int dd = 0; dd < 6; dd++){
        int dir = dd >> 1;
        start[dir] = (block_size[dir] + 1) * (dd & 1);
        lm[dir] = (dd & 1) ? (block_size[dir] + 2) : 1;
        f[dir] = 1;
        mu[dir] = 0;
        for(int k = start[2]; k < lm[2]; ++k){
            for(int j = start[1]; j < lm[1]; ++j){
                for(int i = start[0]; i < lm[0]; ++i){
                    buf[i + (j + k * (block_size[1] + 2)) * (block_size[0] + 2)] =
                        data[dd][i * mu[0] + (j * mu[1] + k * mu[2] * f[1]) * f[0]];
                }
            }
        }
        f[dir] = lm[dir]--;
        start[dir] = mu[dir] = 1;
    }
}



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
    hx = l_size[0] / ((double)shape[0] * block_size[0]);
    hy = l_size[1] / ((double)shape[1] * block_size[1]);
    hz = l_size[2] / ((double)shape[2] * block_size[2]);

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
    MPI_Cart_shift(grid_comm, 0, 1, &neighb_ranks[left], &neighb_ranks[right]);
    MPI_Cart_shift(grid_comm, 1, 1, &neighb_ranks[front], &neighb_ranks[back]);
    MPI_Cart_shift(grid_comm, 2, 1, &neighb_ranks[down], &neighb_ranks[up]);

    //крайние точки
    for(int dir = 0; dir < 3; ++dir){
        int dd= dir << 1;
        if (coords[dir] == 0) {
            std::fill_n(data[dd], data_sizes[dir], u[dd]);
        }
        else if (coords[dir] == shape[dir] - 1) {
            std::fill_n(data[dd+1], data_sizes[dir], u[dd+1]);
        }
    }

    double max_diff = 1;
    while (max_diff >= eps) {
        MPI_Barrier(MPI_COMM_WORLD); //синхронизация процессов

        //отправка и получение
        tranfer(data, next, data_sizes, coords, shape, neighb_ranks, grid_comm);
        import(buf[0], data, data_sizes, block_size);
    }

    MPI_Finalize();

    for (int i = 0; i < 3; i++) {
        free(data[i]);
    }

    return 0;
}
