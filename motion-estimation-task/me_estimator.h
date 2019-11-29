#pragma once
#include "me_field.h"

#include <array>
#include <cassert>
#include <limits>
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

class MotionEstimator {
public:
    /// Constructor
    MotionEstimator(size_t width, size_t height, unsigned char quality, bool use_half_pixel);

    /// Destructor
    ~MotionEstimator();

    /// Copy constructor (deleted)
    MotionEstimator(const MotionEstimator&) = delete;

    /// Move constructor
    MotionEstimator(MotionEstimator&&) = default;

    /// Copy assignment (deleted)
    MotionEstimator& operator=(const MotionEstimator&) = delete;

    /// Move assignment
    MotionEstimator& operator=(MotionEstimator&&) = default;

    /**
     * Estimate motion between two frames
     *
     * @param[in] cur_Y numpy array (uint8) of pixels of the current frame
     * @param[in] prev_Y numpy array (uint8) of pixels of the previous frame
     * @param[out] MEFielf structure of motion vectors for frame
     */
    MEField Estimate(
        py::array_t<unsigned char> cur_Y,
        py::array_t<unsigned char> prev_Y
    );

    /**
     * Size of the borders added to frames by the template, in pixels.
     * This is the most pixels your motion vectors can extend past the image border.
     */
    static constexpr int BORDER = 16;

    /// Size of a block covered by a motion vector. Do not change.
    static constexpr size_t BLOCK_SIZE = 16;

private:
    /// Frame width (not including borders)
    const size_t width;

    /// Frame height (not including borders)
    const size_t height;

    /// Quality
    const unsigned char quality;

    /// Whether to use half-pixel precision
    const bool use_half_pixel;

    /// Extended frame width (including borders)
    const size_t width_ext;

    /// Number of blocks per X-axis
    const size_t num_blocks_hor;

    /// Number of blocks per Y-axis
    const size_t num_blocks_vert;

    /// Position of the first pixel of the frame in the extended frame
    const size_t first_row_offset;

    /// Me field
    MEField me_field;

    /// width + 2 * BORDER
    size_t width_borders;
    /// height + 2 * BORDER
    size_t height_borders;

    /// Buffers for subpixel frames
    unsigned char* prev_Y_up_borders;
    unsigned char* prev_Y_up_left_borders;
    unsigned char* prev_Y_left_borders;

    /// Buffers for frames
    unsigned char* cur_Y_borders;
    unsigned char* prev_Y_borders;

    /**
     * Estimate motion between two frames
     *
     * @param[in] cur_Y array of pixels of the current frame
     * @param[in] prev_Y array of pixels of the previous frame
     * @param[in] prev_Y_up array of pixels of the previous frame shifted half a pixel up,
     *   only valid if use_half_pixel is true
     * @param[in] prev_Y_left array of pixels of the previous frame shifted half a pixel left,
     *   only valid if use_half_pixel is true
     * @param[in] prev_Y_upleft array of pixels of the previous frame shifted half a pixel up left,
     *   only valid if use_half_pixel is true
     * @param[out] mvectors output array of motion vectors
     */
    void CEstimate(const unsigned char* cur_Y,
                  const unsigned char* prev_Y,
                  const unsigned char* prev_Y_up,
                  const unsigned char* prev_Y_left,
                  const unsigned char* prev_Y_upleft,
                  MEField& mvectors);
};
