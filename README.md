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

---

## Installation

### Prerequisites
* Python 3.8+
* C++17 Compliant Compiler (GCC, Clang, MSVC)
* NumPy $\ge$ 1.20.0

### Build from Source
```
git clone https://github.com/arclaav/natumpy.git
cd natumpy
pip install .
```

# Usage Guide
1. Initialization & Memory View
Nawa exposes its internal C++ memory directly to Python via the Buffer Protocol. Zero-Copy.
```
import natumpy
import numpy as np

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
| target | float64 | The equilibrium point for the atom. |
