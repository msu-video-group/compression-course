import pybind11
from distutils.core import setup, Extension

ext_modules = [
    Extension(
        'me_estimator',
        ['me_estimator.cpp', 'metric.cpp', 'main.cpp', 'me_field.cpp'],
        include_dirs=[pybind11.get_include()],
        language='c++',
        extra_compile_args=['-std=c++11']
    ),
]

setup(
    name='library',
    version='0.0.1',
    author='YOUR NAME', #Please enter your name, group.
    author_email='YOUR EMAIL',
    description='ME estimator template for MSU VideoCourse',
    ext_modules=ext_modules,
    requires=['pybind11']
)