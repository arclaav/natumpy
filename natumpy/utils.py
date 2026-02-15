import numpy as np

def create_orthogonal_vector(dim, seed):
    np.random.seed(seed)
    r = np.random.randn(dim)
    i = np.random.randn(dim)
    mag = np.sqrt(r**2 + i**2) + 1e-9
    return (r/mag) + 1j * (i/mag)

def cosine_similarity(v1, v2):
    dot = np.real(np.sum(v1 * np.conj(v2)))
    return dot

