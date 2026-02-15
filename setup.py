from setuptools import setup, Extension, find_packages
from pathlib import Path
import sys

this_dir = Path(__file__).parent
long_desc = (this_dir / "README.md").read_text(encoding="utf-8")

try:
    import numpy
    np_include = [numpy.get_include()]
except ImportError:
    np_include = []

extra_compile_args = [
    '-std=c++17',
    '-O3',
    '-Wall',
    '-march=native',
    '-ffast-math'
]

if sys.platform == 'darwin':
    extra_compile_args.append('-stdlib=libc++')

module = Extension(
    'natumpy.natcore',
    sources=[
        'natumpy/nat_bindings.cpp',
        'natumpy/nat_engine.cpp'
    ],
    depends=[
        'natumpy/nat_engine.hpp'
    ],
    include_dirs=np_include,
    extra_compile_args=extra_compile_args,
    language='c++'
)

setup(
    name='natumpy',
    version='5.0.8',
    packages=find_packages(),
    ext_modules=[module],
    python_requires='>=3.8',
    install_requires=['numpy>=1.20.0'],

    description='Nawa Causal Engine: Complex-Valued Reservoir Computing based on Harmonic Resonance.',
    long_description=long_desc,
    long_description_content_type='text/markdown',

    author='Eternals',
    author_email='admin@defacer.cloud-ip.cc',
    url='https://github.com/arclaav/natumpy',

    keywords=[
        'ai',
        'reservoir-computing',
        'esn',
        'complex-valued',
        'neural-network',
        'c++',
        'nawa'
    ],

    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Science/Research',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
        'Programming Language :: C++',
        'Programming Language :: Python :: 3',
        'License :: OSI Approved :: MIT License',
        'Operating System :: OS Independent',
    ],

    include_package_data=True,
    zip_safe=False,
)
