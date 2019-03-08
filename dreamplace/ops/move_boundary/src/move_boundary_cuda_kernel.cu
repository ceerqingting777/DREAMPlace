#include <stdio.h>
#include <math.h>
#include <float.h>
#include "cuda_runtime.h"
#include "utility/src/print.h"

template <typename T>
__global__ void computeMoveBoundary(
        T* x_tensor, 
        const T* node_size_x_tensor, 
        const T xl, const T xh, 
        const int num_nodes, 
        const int num_movable_nodes, 
        const int num_filler_nodes
        ) 
{
    for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < num_nodes; i += blockDim.x * gridDim.x) 
    {
        if (i < num_movable_nodes || i >= num_nodes-num_filler_nodes)
        {
            x_tensor[i] = max(xl, x_tensor[i]); 
            x_tensor[i] = min(xh-node_size_x_tensor[i], x_tensor[i]);
        }
    }
}

template <typename T>
int computeMoveBoundaryMapCudaLauncher(
        T* x_tensor, T* y_tensor, 
        const T* node_size_x_tensor, const T* node_size_y_tensor, 
        const T xl, const T yl, const T xh, const T yh, 
        const int num_nodes, 
        const int num_movable_nodes, 
        const int num_filler_nodes
        )
{
    int block_count = 32; 
    int thread_count = 1024; 

    cudaError_t status; 
    cudaStream_t stream_x; 
    cudaStream_t stream_y; 
    status = cudaStreamCreate(&stream_x);
    if (status != cudaSuccess)
    {
        printf("cudaStreamCreate failed for stream_x\n");
        fflush(stdout);
        return 1; 
    }
    status = cudaStreamCreate(&stream_y);
    if (status != cudaSuccess)
    {
        printf("cudaStreamCreate failed for stream_y\n");
        fflush(stdout);
        return 1; 
    }

    computeMoveBoundary<<<block_count, thread_count, 0, stream_x>>>(
            x_tensor, 
            node_size_x_tensor, 
            xl, xh, 
            num_nodes, 
            num_movable_nodes, 
            num_filler_nodes
            );

    computeMoveBoundary<<<block_count, thread_count, 0, stream_y>>>(
            y_tensor, 
            node_size_y_tensor, 
            yl, yh, 
            num_nodes, 
            num_movable_nodes, 
            num_filler_nodes
            );

    /* destroy stream */
    status = cudaStreamDestroy(stream_x); 
    stream_x = 0;
    if (status != cudaSuccess) 
    {
        printf("stream_x destroy failed\n");
        fflush(stdout);
        return 1;
    }   
    status = cudaStreamDestroy(stream_y); 
    stream_y = 0; 
    if (status != cudaSuccess) 
    {
        printf("stream_y destroy failed\n");
        fflush(stdout);
        return 1;
    }   
    return 0; 
}

#define REGISTER_KERNEL_LAUNCHER(T) \
    int instantiateComputeMoveBoundaryMapLauncher(\
            T* x_tensor, T* y_tensor, \
            const T* node_size_x_tensor, const T* node_size_y_tensor, \
            const T xl, const T yl, const T xh, const T yh, \
            const int num_nodes, \
            const int num_movable_nodes, \
            const int num_filler_nodes \
            )\
    { \
        return computeMoveBoundaryMapCudaLauncher(\
                x_tensor, y_tensor, \
                node_size_x_tensor, node_size_y_tensor, \
                xl, yl, xh, yh, \
                num_nodes, \
                num_movable_nodes, \
                num_filler_nodes \
                );\
    }
REGISTER_KERNEL_LAUNCHER(float);
REGISTER_KERNEL_LAUNCHER(double);