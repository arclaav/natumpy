from . import natcore

class Nat:
    def __init__(self, value, uncertainty=0.0, _ptr=None):
        if _ptr:
            self._ptr = _ptr
        else:
            # Panggil fungsi C++ dari submodule
            self._ptr = natcore.create_entity(float(value), float(uncertainty))

    def __add__(self, other):
        if not isinstance(other, Nat):
            raise TypeError
        new_ptr = natcore.entangle(self._ptr, other._ptr)
        return Nat(0, 0, _ptr=new_ptr)

    def observe(self):
        return natcore.collapse(self._ptr)

    def state(self):
        return natcore.inspect(self._ptr)

    def __repr__(self):
        s = self.state()
        return f"Nat(mu={s['mu']:.4f}, sigma={s['sigma']:.4f}, entropy={s['entropy']:.4f})"

