#include <pybind11/pybind11.h>
#include "me_estimator.h"
#include "me_field.h"
namespace py = pybind11;

PYBIND11_MODULE(me_estimator, m) {
    py::class_<MotionEstimator>(m, "MotionEstimator")
        .def(py::init<size_t, size_t, unsigned char, bool>())
        .def("Estimate", &MotionEstimator::Estimate);
    py::class_<MEField>(m, "MEField")
        .def(py::init<size_t, size_t, size_t>())
        .def("ConvertToOF", &MEField::ConvertToOF)
        .def("Remap", &MEField::Remap);
};