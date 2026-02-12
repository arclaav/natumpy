# NATUMPY

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Python Version](https://img.shields.io/badge/python-3.8%2B-blue)](https://www.python.org/downloads/)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()

**natumpy** is a highly optimized C++ extension for Python, designed to facilitate efficient numerical field simulations and particle physics modeling. It leverages a C++17 core engine exposed through the Python C-API, providing direct memory access via NumPy without the overhead of data copying.

This library is engineered for production environments requiring low-latency physics stepping, thread safety, and seamless integration with the scientific Python ecosystem.

---

## Resources

- **Source Code**: [github.com/arclaav/natumpy](https://github.com/arclaav/natumpy)
- **Official Documentation**: [natumpy-docs.defacer.dedyn.io](http://natumpy-docs.defacer.dedyn.io)

---

## Key Features

* **High-Performance Core**: The simulation logic is implemented entirely in C++17 (`natcore`), utilizing optimized memory structures and loop unrolling where applicable.
* **Zero-Copy NumPy Integration**: Implements the Python Buffer Protocol. The internal state of the C++ engine is directly accessible as a NumPy structured array (`float64`).
* **Thread Safety & Concurrency**: Critical sections are protected by `std::mutex`. Computationally intensive methods (e.g., `step`) release the Python Global Interpreter Lock (GIL), allowing for true multi-threaded execution.
* **Robust Physics Engine**: Built-in support for damping, target-seeking behavior, and coupled resonance dynamics.

---

## Installation

### Prerequisites
* Python 3.8 or higher
* A C++17 compliant compiler (GCC, Clang, or MSVC)
* NumPy $\ge$ 1.20.0

### Building from Source

To install the latest version directly from the repository:

```
git clone https://github.com/arclaav/natumpy.git
cd natumpy
pip install .
```
For development environments (editable install):
```
pip install -e .
```

## Usage Guide
1. Initialization and Data Views
Initialize a field engine and access its memory directly via NumPy.
```
import natumpy
import numpy as np

# Initialize a field with 1,000,000 atoms
field = natumpy.create_field(1_000_000)

# Create a zero-copy view of the data
# Modifying 'data' instantly updates the C++ engine state
data = natumpy.view(field)

# Access specific properties (Structured Array)
# Fields: 'value', 'velocity', 'mass', 'damping', 'target'
print(f"Initial Mass: {data['mass'][0]}")

2. Configuring Physics Properties
You can modify atom properties using vector operations (via NumPy) or atomic operations (via the C++ API).
# Bulk update via NumPy (Fastest for large updates)
data['mass'][:] = 2.5
data['damping'][:] = 0.05

# Atomic update via C++ API (Thread-safe)
# Syntax: set_props(field, index, mass, damping)
natumpy.NatField.set_props(field, 0, 5.0, 0.1)

# Set a target value for an atom to seek
natumpy.NatField.set_target(field, 0, 100.0)

3. Running the Simulation
The step function advances the physics engine. This operation releases the GIL, making it suitable for background threads.
# Run 100 physics cycles with a delta-time (dt) of 0.01
natumpy.NatField.step(field, cycles=100, dt=0.01)

# Check the new state
print(f"New Value: {data['value'][0]}")

4. Advanced Dynamics: Resonance
Couple two atoms together to simulate force transfer or resonance.
# Couple atom at index 0 and atom at index 1 with coupling strength 0.5
natumpy.NatField.resonate(field, 0, 1, 0.5)
```

## API Reference
natumpy.NatField Methods
| Method | Signature | Description |
|---|---|---|
| set_props | (index: int, mass: float, damping: float) | Sets physical properties for a specific atom. |
| set_target | (index: int, target: float) | Sets the target value an atom attracts towards. |
| step | (cycles: int, dt: float) | Advances the simulation. Releases GIL. |
| resonate | (idx_a: int, idx_b: int, strength: float) | Applies interaction force between two atoms. |
| force | (index: int, value: float) | Hard-resets an atom's value and clears velocity. |
| size | () | Returns the number of atoms in the field. |
Data Structure (NAT_DTYPE)
The NumPy view exposes the following structured data type:
| Field | Type | Description |
|---|---|---|
| value | float64 | Current position/value of the atom. |
| velocity | float64 | Current rate of change. |
| mass | float64 | Mass coefficient (affects inertia). |
| damping | float64 | Damping factor (0.0 - 1.0). |
| target | float64 | The equilibrium point for the atom. |
