#pragma once

#include <vector>
#include <cmath>
#include <complex>
#include <algorithm>
#include <iostream>

using Scalar = double;

struct NawaConfig {
    Scalar dt = 0.01;
    Scalar global_damping = 0.05;
    Scalar phase_coupling = 0.1;
    bool normalize_phase = true;
};

struct NawaState {
    size_t size = 0;

    std::vector<Scalar> q_r; 
    std::vector<Scalar> q_i;

    std::vector<Scalar> p_r;
    std::vector<Scalar> p_i;

    std::vector<Scalar> mass;

    void resize(size_t n) {
        size = n;
        q_r.resize(n, 0.0); q_i.resize(n, 0.0);
        p_r.resize(n, 0.0); p_i.resize(n, 0.0);
        mass.resize(n, 1.0);
    }
};

struct NawaEdge {
    int src_idx;
    int dst_idx;
    Scalar weight;
    int type;         
};

struct NawaTopology {
    std::vector<NawaEdge> edges;

    void add_connection(int u, int v, Scalar w, int t) {
        edges.push_back({u, v, w, t});
    }
    
    void clear() { edges.clear(); }
};

class NawaEngine {
public:
    NawaConfig config;
    NawaState state;
    NawaTopology topology;

    NawaEngine(size_t initial_size) {
        state.resize(initial_size);
    }

    void step(int cycles);

    void set_node_state(int idx, Scalar qr, Scalar qi, Scalar pr, Scalar pi);
    
    void connect(int src, int dst, Scalar weight, int type);

    size_t size() const { return state.size; }

    void enforce_unitary_constraints();

private:
    void compute_forces(std::vector<Scalar>& force_r, std::vector<Scalar>& force_i);
};

