#pragma once

#include <vector>
#include <limits>
#include <memory>
#include <cassert>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

enum class ShiftDir
{
    NONE,
    UP,
    LEFT,
    UPLEFT
};

struct ShiftDirHash
{
    template <typename T>
    std::size_t operator()(T t) const {
        return static_cast<std::size_t>(t);
    }
};

class MV
{
public:
    /// Constructor
    MV(int x = 0,
       int y = 0,
       ShiftDir shift_dir = ShiftDir::NONE,
       long error = std::numeric_limits<long>::max())
        : x(x)
        , y(y)
        , shift_dir(shift_dir)
        , error(error)
        , subvectors(nullptr)
        , is_splitted(false)
    {}

    /// Copy constructor
    MV(const MV& other)
        : x(other.x)
        , y(other.y)
        , shift_dir(other.shift_dir)
        , error(other.error)
        , subvectors(other.subvectors)
        , is_splitted(other.is_splitted)
    {
        if (is_splitted) {
            subvectors = new MV[4];
            for (int i = 0; i < 4; ++i) {
                subvectors[i] = other.subvectors[i];
            }
        }
    }

    /// Move constructor
    MV(MV&& other)
        : MV()
    {
        swap(other);
    }

    /// Copy + move assignment operator
    MV& operator=(MV other)
    {
        swap(other);
        return *this;
    }

    /// Split into 4 subvectors
    inline void Split()
    {
        if (!is_splitted) {
            is_splitted = true;
            subvectors = new MV[4];
        }
    }

    /// Unsplit
    inline void Unsplit()
    {
        is_splitted = false;
        delete[] subvectors;
    }

    /// Check if the motion vector is split
    inline bool IsSplit() const
    {
        return is_splitted;
    }

    /// Get a subvector
    inline MV& SubVector(int id)
    {
        assert(is_splitted && id >= 0 && id < 4);
        return subvectors[id];
    }

    ~MV() {
        if (is_splitted) {
            delete[] subvectors;
        }
    }
    int x;
    int y;
    ShiftDir shift_dir;
    long error;

private:
    MV* subvectors;
    bool is_splitted;

    void swap(MV& other)
    {
        std::swap(x, other.x);
        std::swap(y, other.y);
        std::swap(shift_dir, other.shift_dir);
        std::swap(error, other.error);
        std::swap(subvectors, other.subvectors);
        std::swap(is_splitted, other.is_splitted);
    }
    friend class MEField;
};


class MEField {
private:
    std::vector<MV> vectors;
    size_t block_size;
    size_t num_blocks_vert;
    size_t num_blocks_hor;
public:
    MEField(size_t num_blocks_hor, size_t num_blocks_vert, size_t block_size);

    void set_mv(size_t x, size_t y, const MV& vector);
    MV& get_mv(size_t x, size_t y);
    py::tuple ConvertToOF() const;
    py::array_t<unsigned char> Remap(py::array_t<unsigned char> input) const;
};