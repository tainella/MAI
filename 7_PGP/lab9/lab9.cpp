#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <iomanip>
#include <string.h>
#include "mpi.h"
#include "omp.h"

int is_main(int worker) {
    return worker ? 0 : 1;
}

double u_next(double ux0, double ux1, double uy0, double uy1, double uz0, double uz1, double h2x, double h2y, double h2z) {
    double ans = (ux0 + ux1) * h2x;
    ans += (uy0 + uy1) * h2y;
    ans += (uz0 + uz1) * h2z;
    return ans;
}

double max_determine(double val1, double val2, double curr_max) {
    double diff = val1 - val2;
    diff = diff < 0.0 ? -diff : diff;

    return diff > curr_max ? diff : curr_max;
}

void recv_waiting(MPI_Request* in, MPI_Request* out) {
    MPI_Status temp;
    MPI_Wait(in, &temp);
    MPI_Wait(out, &temp);
}

void print_line(std::ostream& os, double* line, int size) {
    for(int i = 0; i < size; i++){
        os << line[i] << " ";
    }
}

void import_edges(double* inner_buff, double** edge_buffs, int* edge_sizes, int* blocks) {
    int size_x = blocks[0] + 2;
    int size_y = blocks[1] + 2;
    int size_z = blocks[2] + 2;

    int start[3];
    int mult[3];
    int lims[3];
    int ones[3];

    lims[0] = ones[0] = size_x;
    lims[1] = ones[1] = size_y;
    lims[2] = ones[2] = size_z;
    --lims[0];
    --lims[1];
    --lims[2];

    mult[0] = mult[1] = mult[2] = 1;
    start[0] = start[1] = start[2] = 1;

    for(int orr = 0; orr < 6; orr++){
        int dir = orr >> 1;
        start[dir] = (blocks[dir] + 1) * (orr & 1);
        lims[dir] = (orr & 1) ? (blocks[dir] + 2) : 1;
        ones[dir] = 1;
        mult[dir] = 0;
 
        for(int k = start[2]; k < lims[2]; k++){
            for(int j = start[1]; j < lims[1]; j++){
                for(int i = start[0]; i < lims[0]; i++){
                    inner_buff[i + (j + k * size_y) * size_x] = edge_buffs[orr][i * mult[0] + (j * mult[1] + k * mult[2] * ones[1]) * ones[0]];
                }
            }
        }
        ones[dir] = lims[dir]--;
        start[dir] = mult[dir] = 1;
    }
}

void export_edges(double* inner_buff, double** edge_buffs, int* edge_sizes, int* blocks){
    int size_x = blocks[0] + 2;
    int size_y = blocks[1] + 2;
    int size_z = blocks[2] + 2;

    int start[3];
    int mult[3];
    int lims[3];
    int ones[3];

    lims[0] = ones[0] = size_x;
    lims[1] = ones[1] = size_y;
    lims[2] = ones[2] = size_z;
    --lims[0];
    --lims[1];
    --lims[2];

    mult[0] = mult[1] = mult[2] = 1;
    start[0] = start[1] = start[2] = 1;

    for(int orr = 0; orr < 6; orr++) {
        int dir = orr >> 1;
        if(orr & 1){
            start[dir] = blocks[dir];
        }

        lims[dir] = (orr & 1) ? blocks[dir] + 1 : 2;
        ones[dir] = 1;
        mult[dir] = 0;
 
        for(int k = start[2]; k < lims[2]; k++) {
            for(int j = start[1]; j < lims[1]; j++) {
                for(int i = start[0]; i < lims[0]; i++) {
                    edge_buffs[orr][i * mult[0] + (j * mult[1] + k * mult[2] * ones[1]) * ones[0]] = inner_buff[i + (j + k*size_y)*size_x];
                }
            }
        }
        ones[dir] = lims[dir] + 1;
        start[dir] = mult[dir] = 1;
    }
}


void edges_exchange(double** edge_buff_in, double** edge_buff_out, int* dir_edge_sizes, int* coords, int* dimens, int* neighb_ranks, MPI_Comm grid_comm){
    MPI_Request in[6];
    MPI_Request out[6];
    for(int dir = 0; dir < 3; dir++) {
        int dir_x_2 = dir << 1;
        if(coords[dir]) {
            MPI_Isend(edge_buff_out[dir_x_2], dir_edge_sizes[dir], MPI_DOUBLE, neighb_ranks[dir_x_2], 0, grid_comm, &out[dir_x_2]);
            MPI_Irecv(edge_buff_in[dir_x_2], dir_edge_sizes[dir], MPI_DOUBLE, neighb_ranks[dir_x_2], 0, grid_comm, &in[dir_x_2]);
        }
        if(coords[dir] < dimens[dir] - 1) {
            MPI_Isend(edge_buff_out[dir_x_2 + 1], dir_edge_sizes[dir], MPI_DOUBLE, neighb_ranks[dir_x_2 + 1], 0, grid_comm, &out[dir_x_2 + 1]);
            MPI_Irecv(edge_buff_in[dir_x_2 + 1], dir_edge_sizes[dir], MPI_DOUBLE, neighb_ranks[dir_x_2 + 1], 0, grid_comm, &in[dir_x_2 + 1]);
        }
    }

    for(int dir = 0; dir < 3; ++dir) {
        int orr = dir << 1;
        if(coords[dir] > 0) {
            recv_waiting(&in[orr], &out[orr]);
        }
        if(coords[dir] < dimens[dir] - 1) {
            recv_waiting(&in[orr + 1], &out[orr + 1]);
        }
    }
}

enum orientation{
        left = 0, right,
        front, back,
        down, up,
    };

int main(int argc, char **argv) {
    int main_worker, proc_rank;
    int workers_count = 0;
    int dimens[3];
    int blocks[3];
    double l[3];
    double u[6];
    double u0;
    double eps;
    std::string path;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    main_worker = is_main(proc_rank);

    if(main_worker) {
        std::cin >> dimens[0] >> dimens[1] >> dimens[2] >> blocks[0] >> blocks[1] >> blocks[2];
        std::cin >> path >> eps >> l[0] >> l[1] >> l[2] >> u[down] >> u[up] >> u[left] >> u[right];
        std::cin >> u[front] >> u[back] >> u0;
    }

    MPI_Bcast(dimens, 3, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(blocks, 3, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(l, 3, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(u, 6, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&u0, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&eps, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Comm grid_comm;
    int pereod[3], coords[3];
    int neighb_ranks[6];

    std::fill_n(pereod, 3, 0);

    MPI_Cart_create(MPI_COMM_WORLD, 3, dimens, pereod, false, &grid_comm);
    MPI_Comm_rank(grid_comm, &proc_rank);
    MPI_Comm_size(grid_comm, &workers_count);
    MPI_Cart_coords(grid_comm, proc_rank, 3, coords);
    main_worker = is_main(proc_rank);

    MPI_Cart_shift(grid_comm, 0, 1, &neighb_ranks[left], &neighb_ranks[right]);
    MPI_Cart_shift(grid_comm, 1, 1, &neighb_ranks[front], &neighb_ranks[back]);
    MPI_Cart_shift(grid_comm, 2, 1, &neighb_ranks[down], &neighb_ranks[up]);

    double h2x, h2y, h2z;
    h2x = l[0] / ((double)dimens[0] * blocks[0]);
    h2y = l[1] / ((double)dimens[1] * blocks[1]);
    h2z = l[2] / ((double)dimens[2] * blocks[2]);
    h2x *= h2x;
    h2y *= h2y;
    h2z *= h2z;

    double denuminator = 2.0 * (1.0 / h2x + 1.0 / h2y + 1.0 / h2z);
    h2x = 1.0 / (denuminator * h2x);
    h2y = 1.0 / (denuminator * h2y);
    h2z = 1.0 / (denuminator * h2z);

    int dir_edge_sizes[3];
    int size_x = blocks[0] + 2;
    int size_y = blocks[1] + 2;
    int size_z = blocks[2] + 2;

    dir_edge_sizes[0] = size_y * size_z;
    dir_edge_sizes[1] = size_x * size_z;
    dir_edge_sizes[2] = size_y * size_x;
    
    double* norm_data = new double[workers_count];
    double* edge_buff_out[6];
    double* edge_buff_in[6];
    
    double* buff0;
    double* buff1;
    int buff_size = size_x * size_y * size_z;
    buff0 = new double[buff_size];
    buff1 = new double[buff_size];
    std::fill_n(buff0, buff_size, u0);
    
    for(int dir = 0; dir < 3; dir++) {
        int dir_x_2 = dir << 1;
        edge_buff_out[dir_x_2] = new double[dir_edge_sizes[dir]];
        edge_buff_in[dir_x_2] = new double[dir_edge_sizes[dir]];
        edge_buff_out[dir_x_2 + 1] = new double[dir_edge_sizes[dir]];
        edge_buff_in[dir_x_2 + 1] = new double[dir_edge_sizes[dir]];

        std::fill_n(edge_buff_out[dir_x_2], dir_edge_sizes[dir], u0);
        std::fill_n(edge_buff_out[dir_x_2 + 1], dir_edge_sizes[dir], u0);

        if(!coords[dir]) {
            std::fill_n(edge_buff_in[dir_x_2], dir_edge_sizes[dir], u[dir_x_2]);
        }

        if(coords[dir] == dimens[dir] - 1) {
            std::fill_n(edge_buff_in[dir_x_2 + 1], dir_edge_sizes[dir], u[dir_x_2 + 1]);
        }
    }

    auto idx = [size_x, size_y](int i, int j, int k) {
        return (i + size_x * (j + k * size_y));
    };

    auto next_ijk = [blocks](int& i, int& j, int& k, int step) {
        i += step;
        while(i > blocks[0]){
            i -= blocks[0];
            j++;
        }

        while(j > blocks[1]){
            j -= blocks[1];
            k++;
        }
    };

    int thrd_max = omp_get_max_threads();
    double* max_values = new double[thrd_max];
    std::fill_n(max_values, thrd_max, 0.0);
    omp_set_dynamic(0);

    double max_diff = 0.0;

    do{
        max_diff = 0.0;
        edges_exchange(edge_buff_in, edge_buff_out, dir_edge_sizes, coords, dimens, neighb_ranks, grid_comm);
        import_edges(buff0, edge_buff_in, dir_edge_sizes, blocks);

        #pragma omp parallel
        {
            int num_threads = omp_get_num_threads();
            int thread_idx = omp_get_thread_num();

            int i = 1;
            int j = 1;
            int k = 1;
            next_ijk(i, j, k, thread_idx);

            while(k <= blocks[2]) {
                buff1[idx(i, j, k)] = u_next(buff0[idx(i - 1, j, k)], buff0[idx(i + 1, j, k)], buff0[idx(i, j - 1, k)], buff0[idx(i, j + 1, k)], buff0[idx(i, j, k - 1)], buff0[idx(i, j, k + 1)], h2x, h2y, h2z);
                max_values[thread_idx] = max_determine(buff0[idx(i, j, k)], buff1[idx(i, j, k)], max_values[thread_idx]);
                next_ijk(i, j, k, num_threads);
            }
        }

        export_edges(buff1, edge_buff_out, dir_edge_sizes, blocks);

        for(int thrd = 0; thrd < thrd_max; thrd++) {
            max_diff = max_diff < max_values[thrd] ? max_values[thrd] : max_diff;
            max_values[thrd] = 0.0;
        }
        
        MPI_Allgather(&max_diff, 1, MPI_DOUBLE, norm_data, 1, MPI_DOUBLE, grid_comm);
        max_diff = 0.0;
        for(int i = 0; i < workers_count; i++){
            max_diff = max_diff < norm_data[i] ? norm_data[i] : max_diff;
        }

        double* tmp = buff1;
        buff1 = buff0;
        buff0 = tmp;
    } while(max_diff >= eps);

    std::ofstream fout;
    if(main_worker) {
        fout.open(path, std::ofstream::out);
        fout << std::scientific << std::setprecision(7);
    }

    int temp_coord[3];
    MPI_Status status;

    for(int k = 0; k < dimens[2]; k++){
        temp_coord[2] = k;
        for(int kk = 1; kk <= blocks[2]; kk++){
            for(int j = 0; j < dimens[1]; j++){
                temp_coord[1] = j;
                for(int jj = 1; jj <= blocks[1]; jj++){
                    for(int i = 0; i < dimens[0]; i++){
                        temp_coord[0] = i;
                        if(main_worker){
                            if(coords[2] == k && coords[1] == j && coords[0] == i){
                                print_line(fout, &buff0[idx(1, jj, kk)], blocks[0]);
                            }
                            else{
                                int rank = 0;
                                MPI_Cart_rank(grid_comm, temp_coord, &rank);
                                MPI_Recv(buff1, blocks[0], MPI_DOUBLE, rank, 0, grid_comm, &status);
                                print_line(fout, buff1, blocks[0]);
                            }
                        }
                        else{
                            if(coords[2] == k && coords[1] == j && coords[0] == i){
                                MPI_Send(&buff0[idx(1, jj, kk)], blocks[0], MPI_DOUBLE, 0, 0, grid_comm);
                            }
                        }
                    }
                }
            }
        }
    }

    MPI_Finalize();

    delete[] norm_data;
    delete[] buff0;
    delete[] buff1;
    delete[] max_values;

    for(int dir = 0; dir < 3; dir++){
        int dir_x_2 = dir << 1;
        delete[] edge_buff_out[dir_x_2];
        delete[] edge_buff_in[dir_x_2];
        delete[] edge_buff_out[dir_x_2 + 1];
        delete[] edge_buff_in[dir_x_2 + 1];
    }

    if(main_worker){
        fout << "\n";
        fout.close();
    }

    return 0;
}
