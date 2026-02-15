import numpy as np

class Model:
    def __init__(self):
        self.layers = {}
        
    def add_layer(self, name, layer):
        self.layers[name] = layer
        
    def compile(self, beta=10.0, dt=0.2):
        for name, layer in self.layers.items():
            layer.set_config(beta=beta, dt=dt)
            print(f"   [Natumpy] Layer '{name}' configured.")

    def summary(self):
        print("\n🧠 NATUMPY MODEL ARCHITECTURE")
        print("="*50)
        for name, layer in self.layers.items():
            t_name = layer.__class__.__name__
            dim = layer.dim
            if hasattr(layer.engine, 'memory_count'):
                mem = layer.engine.memory_count
                rule = layer.engine.rule_count
                print(f"   - {name:<12} | Type: {t_name:<16} | Dim: {dim} | Mem: {mem} | Rules: {rule}")
        print("="*50 + "\n")

