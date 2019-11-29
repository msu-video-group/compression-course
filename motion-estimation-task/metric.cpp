#include "metric.h"
#include <cmath>

int GetErrorSAD(const unsigned char* block1, const unsigned char *block2, const int stride, const int block_size) {
    int sum = 0;
    for (int i = 0; i < block_size; ++i) {
        for (int j = 0; j < block_size; ++j) {
            sum += abs(int(block1[i * stride + j]) - int(block2[i * stride + j]));
        }
    }
    return sum;
}

int GetErrorSAD_16x16(const unsigned char* block1, const unsigned char *block2, const int stride)
{
    return GetErrorSAD(block1, block2, stride, 16);
}

int GetErrorSAD_8x8(const unsigned char* block1, const  unsigned char *block2, const int stride)
{
    return GetErrorSAD(block1, block2, stride, 8);
}
