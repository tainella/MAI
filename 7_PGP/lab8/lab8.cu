#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <iomanip>
#include <string.h>
#include <thrust/extrema.h>
#include <thrust/device_vector.h>
#include "mpi.h"

// размер флота 12 + разделитель + минус = 14

#define next_ijk(i, j, k, step) { \
    i += step;  \
    while(i > n_x){ \
        i -= n_x; \
        j++; \
    } \
    while(j > n_y){ \
        j -= n_y; \
        k++; \
    } \
} \

#define next_ij(i, j, step) { \
    i += step;  \
    while(i > n_x){ \
	i -= n_x; \
	j++; \
} \
} \

#define next_ik(i, k, step) { \
    i += step;  \
    while(i > n_x){ \
        i -= n_x; \
        k++; \
    } \
} \

#define next_jk(j, k, step) { \
    j += step;  \
    while(j > n_y){ \
        j -= n_y; \
        k++; \
    } \
} \

#define idx(i, j, k) ((i) + size_x*((j) + (k)*size_y))

__host__ __device__ double u_next(double ux0, double ux1, double uy0, double uy1, double uz0, double uz1, double h2x, double h2y, double h2z) {
    double ans = (ux0 + ux1) * h2x;
    ans += (uy0 + uy1) * h2y; 
    ans += (uz0 + uz1) * h2z;
    return ans;
}

__host__ __device__ double max_determine(double val1, double val2, double curr_max){
    double diff = val1 - val2;
    diff = diff < 0.0 ? -diff : diff;

    return diff > curr_max ? diff : curr_max;
}

__global__ void import_x(double* inner_buff, double* edge_buff1, double* edge_buff2, int n_x, int n_y, int n_z){
    int size_x = n_x + 2;
    int size_y = n_y + 2;
    int num_threads = blockDim.x * gridDim.x;
    int thread_idx = blockDim.x * blockIdx.x +  threadIdx.x;

    int j = 1;
    int k = 1;
    next_jk(j, k, thread_idx);

    while(k <= n_z) {
        inner_buff[idx(0, j, k)] = edge_buff1[j + k * size_y];
        inner_buff[idx(n_x + 1, j, k)] = edge_buff2[j + k * size_y];
        next_jk(j, k, num_threads);
    }
}

__global__ void import_y(double* inner_buff, double* edge_buff1, double* edge_buff2, int n_x, int n_y, int n_z){
    int size_x = n_x + 2;
    int size_y = n_y + 2;
    int num_threads = blockDim.x * gridDim.x;
    int thread_idx = blockDim.x * blockIdx.x +  threadIdx.x;

    int i = 1;
    int k = 1;
    next_ik(i, k, thread_idx);

    while(k <= n_z) {
        inner_buff[idx(i, 0, k)] = edge_buff1[i + k * size_x];
        inner_buff[idx(i, n_y + 1, k)] = edge_buff2[i + k * size_x];
        next_ik(i, k, num_threads);
    }
}

__global__ void import_z(double* inner_buff, double* edge_buff1, double* edge_buff2, int n_x, int n_y, int n_z) {
    int size_x = n_x + 2;
    int size_y = n_y + 2;
    int num_threads = blockDim.x * gridDim.x;
    int thread_idx = blockDim.x * blockIdx.x +  threadIdx.x;

    int i = 1;
    int j = 1;
    next_ij(i, j, thread_idx);

    while(j <= n_y) {
        inner_buff[idx(i, j, 0)] = edge_buff1[i + j * size_x];
        inner_buff[idx(i, j, n_z + 1)] = edge_buff2[i + j * size_x];
        next_ij(i, j, num_threads);
    }
}

__global__ void export_x(double* inner_buff, double* edge_buff1, double* edge_buff2, int n_x, int n_y, int n_z) {
    int size_x = n_x + 2;
    int size_y = n_y + 2;
    int num_threads = blockDim.x * gridDim.x;
    int thread_idx = blockDim.x * blockIdx.x +  threadIdx.x;

    int j = 1;
    int k = 1;
    next_jk(j, k, thread_idx);

    while(k <= n_z){
        edge_buff1[j + k * size_y] = inner_buff[idx(1, j, k)];
        edge_buff2[j + k * size_y] = inner_buff[idx(n_x, j, k)];
        next_jk(j, k, num_threads);
    }
}

__global__ void export_y(double* inner_buff, double* edge_buff1, double* edge_buff2, int n_x, int n_y, int n_z) {
    int size_x = n_x + 2;
    int size_y = n_y + 2;
    int num_threads = blockDim.x * gridDim.x;
    int thread_idx = blockDim.x * blockIdx.x +  threadIdx.x;

    int i = 1;
    int k = 1;
    next_ik(i, k, thread_idx);

    while(k <= n_z) {
        edge_buff1[i + k * size_x] = inner_buff[idx(i, 1, k)];
        edge_buff2[i + k * size_x] = inner_buff[idx(i, n_y, k)];
        next_ik(i, k, num_threads);
    }
}

__global__ void export_z(double* inner_buff, double* edge_buff1, double* edge_buff2, int n_x, int n_y, int n_z){
    int size_x = n_x + 2;
    int size_y = n_y + 2;
    int num_threads = blockDim.x * gridDim.x;
    int thread_idx = blockDim.x * blockIdx.x +  threadIdx.x;

    int i = 1;
    int j = 1;
    next_ij(i, j, thread_idx);

    while(j <= n_y) {
        edge_buff1[i + j * size_x] = inner_buff[idx(i, j, 1)];
        edge_buff2[i + j * size_x] = inner_buff[idx(i, j, n_z)];
        next_ij(i, j, num_threads);
    }
}

__global__ void new_grid(double* buffer1, double* buffer0, double* max_values, int n_x, int n_y, int n_z, double h2x, double h2y, double h2z) {
    int size_x = n_x + 2;
    int size_y = n_y + 2;

    int num_threads = blockDim.x * gridDim.x;
    int thread_idx = blockDim.x * blockIdx.x +  threadIdx.x;

    int i = 1;
    int j = 1;
    int k = 1;
    next_ijk(i, j, k, thread_idx);
    double temp;
    max_values[thread_idx] = 0.0;

    while(k <= n_z){
    temp = u_next(buffer0[idx(i - 1, j, k)], buffer0[idx(i + 1, j, k)], buffer0[idx(i, j - 1, k)], buffer0[idx(i, j + 1, k)], buffer0[idx(i, j, k - 1)], buffer0[idx(i, j, k + 1)], h2x, h2y, h2z);
        max_values[thread_idx] = max_determine(buffer0[idx(i, j, k)], temp, max_values[thread_idx]);
        buffer1[idx(i, j, k)] = temp;
        next_ijk(i, j, k, num_threads);
    }
}

int is_main(int worker) {
    return worker ? 0 : 1;
} 

void recv_waiting(MPI_Request* in, MPI_Request* out) {
    MPI_Status temp;
    MPI_Wait(in, &temp);
    MPI_Wait(out, &temp);
}

void print_line(std::ostream& os, double* line, int size) {
    for(int i = 0; i < size; ++i){
        os << line[i] << " ";
    }
}

void edges_exchange(double** edge_buff_in, double** edge_buff_out, int* dir_edge_sizes, int* coords, int* dimens, int* neighb, MPI_Comm grid_comm) {
    MPI_Request in[6];
    MPI_Request out[6];

    for(int dir = 0; dir < 3; dir++) {
        int dir_x_2 = dir << 1;
        if(coords[dir]) {
            MPI_Isend(edge_buff_out[dir_x_2], dir_edge_sizes[dir], MPI_DOUBLE, neighb[dir_x_2], 0, grid_comm, &out[dir_x_2]);
            MPI_Irecv(edge_buff_in[dir_x_2], dir_edge_sizes[dir], MPI_DOUBLE, neighb[dir_x_2], 0, grid_comm, &in[dir_x_2]);
        }
        if (coords[dir] < dimens[dir] - 1) {
            MPI_Isend(edge_buff_out[dir_x_2 + 1], dir_edge_sizes[dir], MPI_DOUBLE, neighb[dir_x_2 + 1], 0, grid_comm, &out[dir_x_2 + 1]);
            MPI_Irecv(edge_buff_in[dir_x_2 + 1], dir_edge_sizes[dir], MPI_DOUBLE, neighb[dir_x_2 + 1], 0, grid_comm, &in[dir_x_2 + 1]);
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

int main(int argc, char **argv){
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int main_worker, proc_rank;
    int workers_count = 0;
    int dimens[3], blocks[3];
    double l[3];
    double u[6];
    double u0, eps;
    std::string path;
    int filename_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    main_worker = is_main(proc_rank);

    if(main_worker) {
        std::cin >> dimens[0] >> dimens[1] >> dimens[2] >> blocks[0] >> blocks[1] >> blocks[2];
        std::cin >> path >> eps >> l[0] >> l[1] >> l[2] >> u[down] >> u[up] >> u[left] >> u[right];
        std::cin >> u[front] >> u[back] >> u0;
        filename_size = path.size();
    }

    MPI_Bcast(dimens, 3, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(blocks, 3, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(l, 3, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(u, 6, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&u0, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&eps, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Bcast(&filename_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    path.resize(filename_size);
    MPI_Bcast((char*) path.c_str(), filename_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    MPI_Comm grid_comm;
    int perod[3];
    int coords[3];
    int neighb[6];

    std::fill_n(perod, 3, 0);

    MPI_Cart_create(MPI_COMM_WORLD, 3, dimens, perod, false, &grid_comm);
    MPI_Comm_rank(grid_comm, &proc_rank);
    MPI_Comm_size(grid_comm, &workers_count); 
    MPI_Cart_coords(grid_comm, proc_rank, 3, coords); 
    main_worker = is_main(proc_rank);

    MPI_Cart_shift(grid_comm, 0, 1, &neighb[left], &neighb[right]); 
    MPI_Cart_shift(grid_comm, 1, 1, &neighb[front], &neighb[back]);
    MPI_Cart_shift(grid_comm, 2, 1, &neighb[down], &neighb[up]);

    int device_count;
    cudaGetDeviceCount(&device_count);
    cudaSetDevice(proc_rank % device_count);

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
    double* h_edge_buff_out[6]; 
    double* h_edge_buff_in[6];
    double* h_buff0;
    double* d_edge_buff_out[6]; 
    double* d_edge_buff_in[6];
    double* d_buff0;
    double* d_buff1;
    double* d_maxvalues;

    int buff_size = size_x * size_y * size_z;
    h_buff0 = new double[buff_size];
    std::fill_n(h_buff0, buff_size, u0);
    cudaMalloc((void**) &d_buff0, sizeof(double) * buff_size);
    cudaMalloc((void**) &d_buff1, sizeof(double) * buff_size);
    cudaMemcpy(d_buff0, h_buff0, sizeof(double) * buff_size, cudaMemcpyHostToDevice);
    cudaMalloc((void**) &d_maxvalues, sizeof(double) * 16 * 16);

    for(int dir = 0; dir < 3; dir++){
        int dir_x_2 = dir << 1;
        h_edge_buff_out[dir_x_2] = new double[dir_edge_sizes[dir]];
        h_edge_buff_in[dir_x_2] = new double[dir_edge_sizes[dir]];
        h_edge_buff_out[dir_x_2 + 1] = new double[dir_edge_sizes[dir]];
        h_edge_buff_in[dir_x_2 + 1] = new double[dir_edge_sizes[dir]];

        cudaMalloc((void**) &d_edge_buff_in[dir_x_2], sizeof(double) * dir_edge_sizes[dir]);
        cudaMalloc((void**) &d_edge_buff_in[dir_x_2 + 1], sizeof(double) * dir_edge_sizes[dir]);
        cudaMalloc((void**) &d_edge_buff_out[dir_x_2], sizeof(double) * dir_edge_sizes[dir]);
        cudaMalloc((void**) &d_edge_buff_out[dir_x_2 + 1], sizeof(double) * dir_edge_sizes[dir]);

        std::fill_n(h_edge_buff_out[dir_x_2], dir_edge_sizes[dir], u0);
        std::fill_n(h_edge_buff_out[dir_x_2 + 1], dir_edge_sizes[dir], u0);

        if(!coords[dir]) {
            std::fill_n(h_edge_buff_in[dir_x_2], dir_edge_sizes[dir], u[dir_x_2]);
        }
        if(coords[dir] == dimens[dir] - 1) {
            std::fill_n(h_edge_buff_in[dir_x_2 + 1], dir_edge_sizes[dir], u[dir_x_2 + 1]);
        }
    }

    double max_diff = 0.0;
    thrust::device_ptr<double> i_ptr = thrust::device_pointer_cast(d_maxvalues);

    do {
        edges_exchange(h_edge_buff_in, h_edge_buff_out, dir_edge_sizes, coords, dimens, neighb, grid_comm);

        for(int orr = left; orr <= up; orr++){
            cudaMemcpy(d_edge_buff_in[orr], h_edge_buff_in[orr], sizeof(double) * dir_edge_sizes[orr >> 1], cudaMemcpyHostToDevice);
        }

        import_x<<<16, 16>>>(d_buff0, d_edge_buff_in[left], d_edge_buff_in[right], blocks[0], blocks[1], blocks[2]);
        import_y<<<16, 16>>>(d_buff0, d_edge_buff_in[front], d_edge_buff_in[back], blocks[0], blocks[1], blocks[2]);
        import_z<<<16, 16>>>(d_buff0, d_edge_buff_in[down], d_edge_buff_in[up], blocks[0], blocks[1], blocks[2]);
        
        cudaThreadSynchronize();

        new_grid<<<16, 16>>>(d_buff1, d_buff0, d_maxvalues, blocks[0], blocks[1], blocks[2], h2x, h2y, h2z);

        cudaThreadSynchronize();

        export_x<<<16, 16>>>(d_buff1, d_edge_buff_out[left], d_edge_buff_out[right], blocks[0], blocks[1], blocks[2]);
        export_y<<<16, 16>>>(d_buff1, d_edge_buff_out[front], d_edge_buff_out[back], blocks[0], blocks[1], blocks[2]);
        export_z<<<16, 16>>>(d_buff1, d_edge_buff_out[down], d_edge_buff_out[up], blocks[0], blocks[1], blocks[2]);

        cudaThreadSynchronize();

        for(int orr = left; orr <= up; orr++){
            cudaMemcpy(h_edge_buff_out[orr], d_edge_buff_out[orr], sizeof(double) * dir_edge_sizes[orr >> 1], cudaMemcpyDeviceToHost);
        }

        max_diff = *thrust::max_element(i_ptr, i_ptr + 16 * 16);

        MPI_Allgather(&max_diff, 1, MPI_DOUBLE, norm_data, 1, MPI_DOUBLE, grid_comm);

        max_diff = 0.0;
        for(int i = 0; i < workers_count; i++){
            max_diff = max_diff < norm_data[i] ? norm_data[i] : max_diff;
        }

        double* tmp = d_buff1;
        d_buff1 = d_buff0;
        d_buff0 = tmp;
    } while(max_diff >= eps);

    cudaMemcpy(h_buff0, d_buff0, sizeof(double) * buff_size, cudaMemcpyDeviceToHost);

    char* write_data = new char[buff_size * 14];
    for(int k = 1; k <= blocks[2]; k++){
        for(int j = 1; j <= blocks[1]; j++){
            int i, len;
            for(i = 1; i < blocks[0]; i++){
                len = sprintf(&write_data[idx(i, j, k) * 14], "%.*e ", 6, h_buff0[idx(i, j, k)]);

                if (len < 14) {
                    write_data[idx(i, j, k) * 14 + len] = ' ';
                }
            }
            len = sprintf(&write_data[idx(i, j, k) * 14], "%.*e\n", 6, h_buff0[idx(i, j, k)]);
            if(len < 14){
                write_data[idx(i, j, k) * 14 + len] = '\n';
            }
        }
    }
    MPI_Datatype float_r;
    MPI_Type_contiguous(14, MPI_CHAR, &float_r); 
    MPI_Type_commit(&float_r); 

    MPI_Datatype local_array, global_array;
    int sizes[3];
    int starts[3];
    int gsizes[3];
    int gstarts[3];

    sizes[0] = size_x;
    sizes[1] = size_y;
    sizes[2] = size_z;
    starts[0] = starts[1] = starts[2] = 1;

    gsizes[0] = blocks[0] * dimens[0];
    gsizes[1] = blocks[1] * dimens[1];
    gsizes[2] = blocks[2] * dimens[2];

    gstarts[0] = blocks[0] * coords[0];
    gstarts[1] = blocks[1] * coords[1];
    gstarts[2] = blocks[2] * coords[2];

    MPI_Type_create_subarray(3, sizes, blocks, starts, MPI_ORDER_FORTRAN, float_r, &local_array);
    MPI_Type_create_subarray(3, gsizes, blocks, gstarts, MPI_ORDER_FORTRAN, float_r, &global_array);
    MPI_Type_commit(&local_array);
    MPI_Type_commit(&global_array);

    MPI_File fh;
    MPI_File_delete(path.c_str(), MPI_INFO_NULL);
    MPI_File_open(grid_comm, path.c_str(), MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
    MPI_File_set_view(fh, 0, MPI_CHAR, global_array, "native", MPI_INFO_NULL);
    MPI_File_write_all(fh, write_data, 1, local_array, MPI_STATUS_IGNORE);
    MPI_File_close(&fh);
    MPI_Finalize();

    delete[] norm_data;
    delete[] h_buff0;
    delete[] write_data;
    cudaFree(d_buff0);
    cudaFree(d_buff1);
    cudaFree(d_maxvalues);

    for(int dir = 0; dir < 3; dir++){
        int dir_x_2 = dir << 1;
        delete[] h_edge_buff_out[dir_x_2];
        cudaFree(d_edge_buff_out[dir_x_2]);
        delete[] h_edge_buff_in[dir_x_2];
        cudaFree(d_edge_buff_in[dir_x_2]);
        delete[] h_edge_buff_out[dir_x_2 + 1];
        cudaFree(d_edge_buff_out[dir_x_2 + 1]);
        delete[] h_edge_buff_in[dir_x_2 + 1];
        cudaFree(d_edge_buff_in[dir_x_2 + 1]);
    }    

    return 0;
}