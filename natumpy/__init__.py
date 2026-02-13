import numpy as np
from . import natcore

NawaEngine = natcore.NawaEngine

def create_engine(size):
    return NawaEngine(int(size))

def get_state_complex(engine):
    real = engine.q_r
    imag = engine.q_i
    return real + 1j * imag

