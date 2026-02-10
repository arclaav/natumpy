from setuptools import setup, Extension, find_packages

module = Extension(
    'natumpy.natcore',
    sources=['natumpy/natcore.cpp'],
    extra_compile_args=['-std=c++17', '-O3']
)

setup(
    name='natumpy',
    version='1.0.0',
    packages=find_packages(),
    ext_modules=[module]
)

