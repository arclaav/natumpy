import numpy as np

from . import natcore

from . import layers
from . import model
from . import utils
from . import text

__version__ = "5.0.8"

NawaEngine = natcore.NawaSystem

Model = model.Model
BaseLayer = layers.BaseLayer
ReservoirLayer = layers.ReservoirLayer
ReadoutLayer = layers.ReadoutLayer

create_orthogonal_vector = utils.create_orthogonal_vector
cosine_similarity = utils.cosine_similarity

def create_engine(size):
    return NawaEngine(int(size))

def get_state_complex(engine, layer_idx=0):
    real, imag = engine.get_state(int(layer_idx))
    return real + 1j * imag
