import numpy as np
import pickle
import os

class ResonantTokenizer:
    def __init__(self, dim=128):
        self.dim = dim
        self.vocab_size = 256
        
        rng = np.random.default_rng(42)
        
        r = rng.normal(0, 1, (self.vocab_size, dim))
        i = rng.normal(0, 1, (self.vocab_size, dim))
        
        mag = np.sqrt(r**2 + i**2) + 1e-9
        self.embeddings_r = r / mag
        self.embeddings_i = i / mag
        
    def encode(self, text):
        if isinstance(text, str):
            bytes_data = text.encode('utf-8')
        else:
            bytes_data = text
            
        indices = np.array(list(bytes_data), dtype=int)
        
        seq_r = self.embeddings_r[indices]
        seq_i = self.embeddings_i[indices]
        
        return seq_r, seq_i

    def decode(self, vec_r, vec_i):
        scores = (vec_r @ self.embeddings_r.T) + (vec_i @ self.embeddings_i.T)
        best_idx = np.argmax(scores)
        return best_idx

    def decode_sequence(self, seq_r, seq_i):
        bytes_list = []
        for i in range(len(seq_r)):
            idx = self.decode(seq_r[i], seq_i[i])
            bytes_list.append(idx)
        
        try:
            return bytes(bytes_list).decode('utf-8', errors='replace')
        except:
            return "<binary>"

    def save(self, filename):
        with open(filename, 'wb') as f:
            pickle.dump({
                'dim': self.dim,
                'er': self.embeddings_r,
                'ei': self.embeddings_i
            }, f)
            
    def load(self, filename):
        if not os.path.exists(filename): return
        with open(filename, 'rb') as f:
            data = pickle.load(f)
            self.dim = data['dim']
            self.embeddings_r = data['er']
            self.embeddings_i = data['ei']

