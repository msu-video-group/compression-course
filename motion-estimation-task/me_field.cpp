#include "me_field.h"

MEField::MEField(size_t num_blocks_hor, size_t num_blocks_vert, size_t block_size)
    : vectors(num_blocks_hor * num_blocks_vert)
    , block_size(block_size)
    , num_blocks_vert(num_blocks_vert)
    , num_blocks_hor(num_blocks_hor)
{
}


void MEField::set_mv(size_t x, size_t y, const MV& vector) {
    vectors[y * num_blocks_hor + x] = vector;
}

MV& MEField::get_mv(size_t x, size_t y) {
    return vectors[y * num_blocks_hor + x];
}

size_t clip(int idx, int begin, int end) {
    if (idx < begin) {
        return begin;
    } else if (idx >= end) {
        return end - 1;
    } else {
        return idx;
    }
}

py::array_t<unsigned char> MEField::Remap(py::array_t<unsigned char> input) const {
    size_t height = num_blocks_vert * block_size;
    size_t width = num_blocks_hor * block_size;
    
    unsigned char *input_ptr = (unsigned char *)input.request().ptr;

    py::array_t<double> result(height * width);
    double* result_buf = (double* )result.request().ptr;
    
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            const auto i = (y / block_size);
            const auto j = (x / block_size);
            auto mv = vectors[i * num_blocks_hor + j];
            if (mv.IsSplit()) {
                const auto h = (((y % block_size) < (block_size / 2)) ? 0 : 2)
                    + (((x % block_size) < (block_size / 2)) ? 0 : 1);
                mv = mv.SubVector(h);
            }
            int reference_y = clip(y + mv.y, 0, height);
            int reference_x = clip(x + mv.x, 0, width);
            switch (mv.shift_dir) {
                default:
                case ShiftDir::NONE:
                    result_buf[y * width + x] = input_ptr[reference_y * width + reference_x];
                    break;
                case ShiftDir::UP:
                    if (reference_y > 0) {
                        result_buf[y * width + x] = (int(input_ptr[(reference_y - 1) * width + reference_x]) + input_ptr[reference_y * width + reference_x]) / 2;
                    } else {
                        result_buf[y * width + x] = input_ptr[reference_y * width + reference_x];
                    }
                    break;

                case ShiftDir::LEFT:
                    if (reference_x > 0) {
                        result_buf[y * width + x] = (int(input_ptr[reference_y * width + reference_x]) + input_ptr[reference_y * width + reference_x - 1]) / 2;
                    } else {
                        result_buf[y * width + x] = input_ptr[reference_y * width + reference_x];
                    }
                    break;

                case ShiftDir::UPLEFT:
                    if (reference_x > 0 && reference_y > 0) {
                        result_buf[y * width + x] = (
                            int(input_ptr[reference_y * width + reference_x]) + 
                            input_ptr[(reference_y - 1) * width + reference_x] + 
                            input_ptr[(reference_y - 1) * width + reference_x - 1] + 
                            input_ptr[reference_y * width + reference_x - 1]
                        ) / 4;
                    } else if (reference_x > 0) {
                        result_buf[y * width + x] = (int(input_ptr[reference_y * width + reference_x]) + input_ptr[reference_y * width + reference_x - 1]) / 2;
                    } else if (reference_y > 0) {
                        result_buf[y * width + x] = (int(input_ptr[(reference_y - 1) * width + reference_x]) + input_ptr[reference_y * width + reference_x]) / 2;
                    } else {
                        result_buf[y * width + x] = input_ptr[reference_y * width + reference_x];
                    }
                    break;
                }
            }
    }
    result.resize({height, width});
    return result;
}


py::tuple MEField::ConvertToOF() const {
    size_t height = num_blocks_vert * block_size;
    size_t width = num_blocks_hor * block_size;
    int delta_x = 0;
    int delta_y = 0;
    py::array_t<double> result_x(height * width);
    py::array_t<double> result_y(height * width);
    double* result_x_buf = (double* )result_x.request().ptr;
    double* result_y_buf = (double* )result_y.request().ptr;
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            const auto i = (y / block_size);
            const auto j = (x / block_size);
            auto mv = vectors[i * num_blocks_hor + j];
            if (mv.IsSplit()) {
                const auto h = (((y % block_size) < (block_size / 2)) ? 0 : 2)
                    + (((x % block_size) < (block_size / 2)) ? 0 : 1);
                mv = mv.SubVector(h);
            }
            switch (mv.shift_dir) {
                default:
                case ShiftDir::NONE:
                    delta_x = 0;
                    delta_y = 0;
                    break;

                case ShiftDir::UP:
                    delta_x = 0;
                    delta_y = -0.5;
                    break;

                case ShiftDir::LEFT:
                    delta_x = -0.5;
                    delta_y = 0;
                    break;

                case ShiftDir::UPLEFT:
                    delta_x = -0.5;
                    delta_y = -0.5;
                    break;
                }
                result_x_buf[y * width + x] = mv.x + delta_x;
                result_y_buf[y * width + x] = mv.y + delta_y;
            }
    }
    result_x.resize({height, width});
    result_y.resize({height, width});
    return py::make_tuple(result_y, result_x);
};