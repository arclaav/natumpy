<<<<<<< HEAD
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Python: 3.8+](https://img.shields.io/badge/python-3.8%2B-blue)](https://www.python.org/downloads/)
[![Architecture: Nawa](https://img.shields.io/badge/Architecture-Complex%20Valued-purple)]()

> **"Beyond Probability. Into Dynamics."**

**natumpy** is a high-performance C++ extension implementing the **Nawa Architecture**: a deterministic, complex-valued dynamical system designed for next-generation AI reasoning.

Unlike traditional neural networks that rely on statistical inference (matrix multiplication), Nawa models intelligence as **coupled oscillators** on a high-dimensional graph. It leverages **Symplectic Integration** (Hamiltonian Mechanics) to ensure energy conservation and reversible logic, and uses **Complex Numbers** to enable signal superposition and phase-dependent gating.

---

## Core Concepts

### 1. The "Superposition" Hijack
Standard AI uses scalar values ($x$). Nawa uses **Complex State Vectors** ($z = r + i\theta$).
* **Real Component ($r$):** Represents Magnitude/Activation strength.
* **Imaginary Component ($\theta$):** Represents Phase/Timing.
* **Benefit:** Allows orthogonal information processing. Two signals can travel through the same node without corruption if their phases are orthogonal.

### 2. Symplectic Integrator
We do not use simple Euler integration. Nawa employs a **Semi-Implicit Symplectic Solver**.
* **Stability:** Guarantees that the phase-space volume is preserved.
* **Reversibility:** The system logic can be reversed in time ($t \to -t$) to trace cause-and-effect.

### 3. Struct of Arrays (SoA)
Optimized for modern CPU Cache and SIMD (Single Instruction, Multiple Data). Data is separated into contiguous memory blocks (`q_r`, `q_i`, `p_r`, `p_i`) ensuring maximum throughput during physics stepping.
=======
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
>>>>>>> 2ab1e845db617a91ec8da8e83bf2d6945013c182

---

## Installation

### Prerequisites
<<<<<<< HEAD
* Python 3.8+
* C++17 Compliant Compiler (GCC, Clang, MSVC)
* NumPy $\ge$ 1.20.0

### Build from Source
=======
* Python 3.8 or higher
* A C++17 compliant compiler (GCC, Clang, or MSVC)
* NumPy $\ge$ 1.20.0

### Building from Source

To install the latest version directly from the repository:

>>>>>>> 2ab1e845db617a91ec8da8e83bf2d6945013c182
```
git clone https://github.com/arclaav/natumpy.git
cd natumpy
pip install .
```
<<<<<<< HEAD

# Usage Guide
1. Initialization & Memory View
Nawa exposes its internal C++ memory directly to Python via the Buffer Protocol. Zero-Copy.
=======
For development environments (editable install):
```
pip install -e .
```

## Usage Guide
1. Initialization and Data Views
Initialize a field engine and access its memory directly via NumPy.
>>>>>>> 2ab1e845db617a91ec8da8e83bf2d6945013c182
```
import natumpy
import numpy as np

<<<<<<< HEAD
# Initialize the Nawa Engine with 1,000 nodes
# This allocates aligned memory for Real/Imaginary states
engine = natumpy.NawaEngine(1000)

# Access the raw memory views (Modifying these updates the C++ engine instantly)
# q_r = Position (Real), q_i = Position (Imaginary/Phase)
q_real = engine.q_r
q_imag = engine.q_i

# Initialize a specific state (e.g., Node 0)
# Set Magnitude to 1.0, Phase to 0.0
q_real[0] = 1.0
q_imag[0] = 0.0

2. Wiring the Topology
Connect nodes to create logic gates or resonant pathways.
# connect(source, destination, weight, type)
# Type 0: Standard Coupling (Spring-like)
# Type 2: Phase Coupling (Resonance-only)

# Connect Node 0 to Node 1 with Phase Resonance
engine.connect(0, 1, weight=0.5, type=2)

3. Running the Dynamics
Advance the Hamiltonian system.
# Run 100 physics cycles
engine.step(100)

# Observe the result
## Did the signal propagate? Did the phase shift?
print(f"Node 1 State: {q_real[1]:.4f} + {q_imag[1]:.4f}j")
```

# Stability & Unitary Constraints
Because Nawa operates on continuous physics, numerical drift (floating-point errors) is a reality. The engine includes a mechanism to mitigate this:
Unitary Constraint (enforce_unitary_constraints):
If enabled, the engine normalizes the magnitude of the complex vector at each step (|z| \to 1.0), ensuring that information is stored purely in the Phase. This prevents "exploding gradients" or signal decay in long-running simulations.
# API Reference
natumpy.NawaEngine(size: int)
Creates a new simulation context.
Methods
| Method | Description |
|---|---|
| step(cycles: int) | Advances the symplectic integrator. Releases GIL for threading. |
| connect(u, v, w, type) | Creates a directed edge from node u to v. |
| set_node_state(...) | Atomic setter for q_r, q_i, p_r, p_i. |
| enforce_unitary_constraints() | Manually triggers normalization to fix numerical drift. |
Properties (NumPy Arrays)
| Property | Type | Physics Meaning |
|---|---|---|
| q_r | float64[] | Generalized Coordinate (Real Part) |
| q_i | float64[] | Generalized Coordinate (Imaginary Part) |
| p_r | float64[] | Conjugate Momentum (Real Part) |
| p_i | float64[] | Conjugate Momentum (Imaginary Part) |
| mass | float64[] | Inertial Coefficient |
ng | float64 | Damping factor (0.0 - 1.0). |
=======
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
>>>>>>> 2ab1e845db617a91ec8da8e83bf2d6945013c182
| target | float64 | The equilibrium point for the atom. |
