#pragma once

#include <cstdint>

/// Compute SAD between two blocks
int GetErrorSAD(const unsigned char* block1, const unsigned char *block2, const int stride, const int block_size);

/// Compute SAD between two 16x16 blocks
int GetErrorSAD_16x16(const unsigned char* block1, const unsigned char *block2, const int stride);

/// Compute SAD between two 8x8 blocks
int GetErrorSAD_8x8(const unsigned char* block1, const unsigned char *block2, const int stride);
