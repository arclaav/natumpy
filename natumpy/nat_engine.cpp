#include "nat_engine.hpp"

void NawaEngine::set_node_state(int idx, Scalar qr, Scalar qi, Scalar pr, Scalar pi) {
    if (idx >= 0 && idx < (int)state.size) {
        state.q_r[idx] = qr;
        state.q_i[idx] = qi;
        state.p_r[idx] = pr;
        state.p_i[idx] = pi;
    }
}

void NawaEngine::connect(int src, int dst, Scalar weight, int type) {
    if (src >= 0 && src < (int)state.size && dst >= 0 && dst < (int)state.size) {
        topology.add_connection(src, dst, weight, type);
    }
}

void NawaEngine::enforce_unitary_constraints() {
    for (size_t i = 0; i < state.size; ++i) {
        Scalar mag_sq = state.q_r[i] * state.q_r[i] + state.q_i[i] * state.q_i[i];
        if (mag_sq > 1.0) {
            Scalar scale = 1.0 / std::sqrt(mag_sq);
            state.q_r[i] *= scale;
            state.q_i[i] *= scale;
            state.p_r[i] *= 0.5; 
            state.p_i[i] *= 0.5;
        }
    }
}

void NawaEngine::compute_forces(std::vector<Scalar>& force_r, std::vector<Scalar>& force_i) {
    std::fill(force_r.begin(), force_r.end(), 0.0);
    std::fill(force_i.begin(), force_i.end(), 0.0);

    for (const auto& edge : topology.edges) {
        int u = edge.src_idx;
        int v = edge.dst_idx;
        Scalar w = edge.weight;

        Scalar dr = state.q_r[v] - state.q_r[u];
        Scalar di = state.q_i[v] - state.q_i[u];

        if (edge.type == 2) { 
            Scalar dot = state.q_r[u] * state.q_r[v] + state.q_i[u] * state.q_i[v];
            Scalar phase_force = (1.0 - dot) * config.phase_coupling;
            
            force_r[u] += phase_force * state.q_r[v];
            force_i[u] += phase_force * state.q_i[v];
            force_r[v] -= phase_force * state.q_r[u];
            force_i[v] -= phase_force * state.q_i[u];
        } else {
            force_r[u] += w * dr;
            force_i[u] += w * di;
            
            force_r[v] -= w * dr;
            force_i[v] -= w * di;
        }
    }
}

void NawaEngine::step(int cycles) {
    size_t n = state.size;
    std::vector<Scalar> fr(n, 0.0);
    std::vector<Scalar> fi(n, 0.0);
    Scalar dt = config.dt;
    Scalar damping_factor = 1.0 - config.global_damping;

    for (int k = 0; k < cycles; ++k) {
        compute_forces(fr, fi);

        for (size_t i = 0; i < n; ++i) {
            Scalar inv_m = 1.0 / state.mass[i];

            state.p_r[i] += fr[i] * dt;
            state.p_i[i] += fi[i] * dt;

            state.p_r[i] *= damping_factor;
            state.p_i[i] *= damping_factor;

            state.q_r[i] += (state.p_r[i] * inv_m) * dt;
            state.q_i[i] += (state.p_i[i] * inv_m) * dt;
        }

        if (config.normalize_phase) {
            enforce_unitary_constraints();
        }
    }
}

