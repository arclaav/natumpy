from setuptools import setup, Extension, find_packages
import sys

try:
    import numpy
    np_include = [numpy.get_include()]
except ImportError:
    np_include = []

extra_compile_args = ['-std=c++17', '-O3', '-Wall']

if sys.platform == 'darwin':
    extra_compile_args.append('-stdlib=libc++')

module = Extension(
    'natumpy.natcore',
    sources=['natumpy/natcore.cpp'],
    include_dirs=np_include,
    extra_compile_args=extra_compile_args,
    language='c++'
)

setup(
    name='natumpy',
    version='3.1.0',
    packages=find_packages(),
    ext_modules=[module],
    python_requires='>=3.8',
    install_requires=['numpy>=1.20.0'],
)
