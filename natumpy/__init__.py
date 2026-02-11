import numpy as np
from . import natcore

NatField = natcore.NatField

NAT_DTYPE = np.dtype([
    ('value', np.float64),
    ('velocity', np.float64),
    ('mass', np.float64),
    ('damping', np.float64),
    ('target', np.float64)
])

def create_field(size):
    return NatField(int(size))

def view(field):
    return np.array(field, copy=False)

