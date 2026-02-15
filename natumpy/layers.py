import numpy as np
from . import natcore

class BaseLayer:
    def __init__(self, dim, name="Layer"):
        self.dim = dim
        self.name = name
        self.engine = natcore.NawaSystem(dim)
        
    def get_state(self):
        r0, i0 = self.engine.get_state(0)
        r4, i4 = self.engine.get_state(4)
        return np.concatenate([r0, i0, r4, i4])

    def step(self, r, i):
        self.engine.step(r, i)

class ReservoirLayer(BaseLayer):
    def forward(self, input_r, input_i):
        self.engine.step(input_r, input_i)
        return self.get_state()

    def save(self, filename):
        self.engine.save(filename)
        
    def load(self, filename):
        self.engine.load(filename)

class ReadoutLayer:
    def __init__(self, input_dim, output_dim, alpha=1.0):
        self.input_dim = input_dim
        self.output_dim = output_dim
        self.alpha = alpha
        self.W_out = None
        
    def fit(self, X_states, Y_targets):
        print(f"   [Readout] Melatih W_out ({X_states.shape} >> {Y_targets.shape})...")
        
        A = X_states.T @ X_states + self.alpha * np.eye(X_states.shape[1])
        B = X_states.T @ Y_targets
        
        try:
            self.W_out = np.linalg.solve(A, B)
            print("   [Readout] Solusi Exact ditemukan.")
        except np.linalg.LinAlgError:
            print("   [Readout] Matrix Singular. Menggunakan PseudoInverse.")
            self.W_out = np.linalg.pinv(A) @ B
            
    def predict(self, state_vec):
        if self.W_out is None:
            raise ValueError("Readout belum dilatih, Panggil .fit() dulu.")
        return state_vec @ self.W_out

    def save(self, filename):
        np.save(filename, self.W_out)
        
    def load(self, filename):
        self.W_out = np.load(filename)

