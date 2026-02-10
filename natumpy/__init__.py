from . import natcore

class Paradox:
    def __init__(self, value=None, _ptr=None):
        if _ptr:
            self._ptr = _ptr
        elif value is not None:
            self._ptr = natcore.create_chaos(float(value))
        else:
            raise ValueError

    def __add__(self, other):
        new_ptr = natcore.provoke(self._ptr, other._ptr)
        return Paradox(_ptr=new_ptr)

    def possibilities(self):
        return natcore.witness(self._ptr)

    def collapse(self, mode="random"):
        strategies = {"average": 0, "optimist": 1, "random": 2}
        return natcore.force_choice(self._ptr, strategies.get(mode, 2))

    def __repr__(self):
        data = self.possibilities()
        count = len(data['potentials'])
        return f"<Paradox | {count} Realities | Chaos Level: {data['chaos_level']}>"
