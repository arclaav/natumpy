from . import natcore

class NatEnvironment:
    def __init__(self):
        self._sys = natcore.create_system()

    def inject(self, value, uncertainty=0.1):
        natcore.inject_data(self._sys, float(value), float(uncertainty))

    def evolve(self, cycles=1):
        natcore.run_cycle(self._sys, int(cycles))

    def observe(self):
        data = natcore.get_state(self._sys)
        return sorted(data, key=lambda x: x['stability'], reverse=True)

    def __repr__(self):
        state = self.observe()
        return f"<NatEnvironment | Population: {len(state)} | Max Stability: {state[0]['stability'] if state else 0.0:.2f}>"

