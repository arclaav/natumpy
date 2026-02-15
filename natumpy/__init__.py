import numpy as np
from . import natcore
from . import layers
from . import model
from . import utils

__version__ = "8.0.0"

NawaEngine = natcore.NawaSystem 

Model = model.Model
BaseLayer = layers.BaseLayer
HolographicLayer = layers.HolographicLayer
CausalLayer = layers.CausalLayer

create_orthogonal_vector = utils.create_orthogonal_vector
cosine_similarity = utils.cosine_similarity

def create_engine(size):
    return NawaEngine(int(size))

def get_state_complex(engine, layer_idx=0):
    real, imag = engine.get_state(int(layer_idx))
    return real + 1j * imag

def train_causality(engine, layer_idx, cause_state, effect_state):
    c_r = np.real(cause_state).astype(np.float64)
    c_i = np.imag(cause_state).astype(np.float64)
    e_r = np.real(effect_state).astype(np.float64)
    e_i = np.imag(effect_state).astype(np.float64)
    
    engine.learn(int(layer_idx), c_r, c_i, e_r, e_i)

def think_next(engine, input_vec):
    ir = np.real(input_vec).astype(np.float64)
    ii = np.imag(input_vec).astype(np.float64)
    engine.step(ir, ii)
