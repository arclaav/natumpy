from setuptools import setup, Extension, find_packages
import sys
import numpy

extra_compile_args = ['-std=c++17', '-O1']

if sys.platform == 'darwin':
    extra_compile_args.append('-stdlib=libc++')

module = Extension(
    'natumpy.natcore',
    sources=['natumpy/natcore.cpp'],
    include_dirs=[numpy.get_include()],
    extra_compile_args=extra_compile_args,
    language='c++'
)

setup(
    name='natumpy',
    version='3.1.0',
    packages=find_packages(),
    ext_modules=[module],
    install_requires=['numpy'],
)

