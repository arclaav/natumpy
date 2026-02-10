from setuptools import setup, Extension, find_packages

module = Extension(
    'natumpy.natcore',
    sources=['natumpy/natcore.cpp']
)

setup(
    name='natumpy',
    version='0.1.0',
    description='Natural Computation Library',
    packages=find_packages(),
    ext_modules=[module]
)

