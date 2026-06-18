
#include <cmath>

#include "print.h"
#include "bunnyIO.h"
#include "device-bunnyMIP.h"

// Forward declaration of CPU MIP function from bunnyMIP.cpp (temporary)
void rotated_mip(const uint16_t* volume, uint16_t* image, int N, int M, const float* R);

// Constant memory for the 3x3x3 Gaussian kernel weights (27 floats).
// Constant memory is cached and broadcast to all threads — ideal for
// small read-only data that every thread reads identically.
__constant__ float c_gauss_kernel[27];


// =============================================================================
// Kernel 1: Threshold
// Each thread handles one voxel. Zeroes it if below threshold.
// =============================================================================
__global__ void kernel_threshold(uint16_t* volume, uint16_t threshold, int size)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= size) return;
    if (volume[i] < threshold)
        volume[i] = 0;
}


// =============================================================================
// Kernel 2: 3D Gaussian Blur
// Each thread handles one output voxel, reading its 3x3x3 neighbourhood.
// Kernel weights are read from constant memory.
// =============================================================================
__global__ void kernel_gaussian_blur(const uint16_t* input, uint16_t* output, int N, int M)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    int z = blockIdx.z * blockDim.z + threadIdx.z;

    if (x >= N || y >= N || z >= M) return;

    float result = 0.0f;
    for (int dz = -1; dz <= 1; dz++) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int nx = max(0, min(N - 1, x + dx));
                int ny = max(0, min(N - 1, y + dy));
                int nz = max(0, min(M - 1, z + dz));
                float w = c_gauss_kernel[(dz + 1) * 9 + (dy + 1) * 3 + (dx + 1)];
                result += (float)input[(size_t)nz * N * N + ny * N + nx] * w;
            }
        }
    }
    output[(size_t)z * N * N + y * N + x] = (uint16_t)result;
}


// =============================================================================
// Device entry point.
// =============================================================================
void device_bunny_mip(const uint16_t* input, uint16_t threshold,
        float sigma, const float* R, uint16_t* output)
{
    print("Running functions on the GPU\n");

    const int volume_size = kBunnySize * kBunnySize * kBunnyN;

    // Allocate device buffer and upload the volume
    uint16_t* d_volume = nullptr;
    cudaMalloc(&d_volume, volume_size * sizeof(uint16_t));
    cudaMemcpy(d_volume, input, volume_size * sizeof(uint16_t), cudaMemcpyHostToDevice);

    // Step 1: Threshold
    print("  gpu: applying threshold\n");
    const int block_size = 256;
    const int grid_size = (volume_size + block_size - 1) / block_size;
    kernel_threshold<<<grid_size, block_size>>>(d_volume, threshold, volume_size);
    cudaDeviceSynchronize();

    // Step 2: Gaussian Blur
    print("  gpu: applying filter\n");

    // Compute normalised 3x3x3 Gaussian kernel on CPU
    float h_kernel[27];
    float kernel_sum = 0.0f;
    for (int dz = -1; dz <= 1; dz++)
        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++) {
                float d2 = (float)(dx*dx + dy*dy + dz*dz);
                float w = expf(-d2 / (2.0f * sigma * sigma));
                h_kernel[(dz + 1) * 9 + (dy + 1) * 3 + (dx + 1)] = w;
                kernel_sum += w;
            }
    for (int i = 0; i < 27; i++)
        h_kernel[i] /= kernel_sum;

    // Upload kernel weights to constant memory
    cudaMemcpyToSymbol(c_gauss_kernel, h_kernel, 27 * sizeof(float));

    // Allocate separate output buffer (blur cannot be done in-place)
    uint16_t* d_blurred = nullptr;
    cudaMalloc(&d_blurred, volume_size * sizeof(uint16_t));

    // Launch blur with a 3D grid: each thread owns one (x, y, z) voxel
    dim3 block(8, 8, 4);   // 256 threads per block
    dim3 grid(
        (kBunnySize + 7) / 8,
        (kBunnySize + 7) / 8,
        (kBunnyN   + 3) / 4
    );
    kernel_gaussian_blur<<<grid, block>>>(d_volume, d_blurred, kBunnySize, kBunnyN);
    cudaDeviceSynchronize();

    cudaFree(d_volume);

    // Step 3: MIP Projection (CPU, temporary)
    print("  gpu: generating MIP\n");
    uint16_t* h_blurred = new uint16_t[volume_size];
    cudaMemcpy(h_blurred, d_blurred, volume_size * sizeof(uint16_t), cudaMemcpyDeviceToHost);
    cudaFree(d_blurred);

    rotated_mip(h_blurred, output, kBunnySize, kBunnyN, R);
    delete[] h_blurred;
}
