#pragma once
#include <vector>
#include <cmath>
#include <complex>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>
#include <memory>

#define NAWA_LAYERS 9 

using Scalar = double;

struct NawaState {
    size_t size = 0;
    std::vector<Scalar> q_r; 
    std::vector<Scalar> q_i;

    void resize(size_t n) {
        size = n;
        q_r.resize(n, 0.0); q_i.resize(n, 0.0);
    }
};

struct NawaMemory {
    size_t dim = 0;
    
    std::vector<Scalar> W_recurrent_r;
    std::vector<Scalar> W_recurrent_i;
    
    std::vector<Scalar> W_input_r, W_input_i;
    std::vector<Scalar> W_gate_r, W_gate_i;
    std::vector<Scalar> W_feedback_r, W_feedback_i;

    void init(size_t dimension);
    void save(std::ostream& out);
    void load(std::istream& in);
};

class NawaSphere {
public:
    int layer_id;
    int time_ratio;
    
    NawaState state;
    NawaMemory memory;
    
    std::vector<Scalar> buf_input_r, buf_input_i;
    std::vector<Scalar> buf_feedback_r, buf_feedback_i;

    NawaSphere(size_t dimension, int id, int ratio);

    void init_weights(int seed);
    void step_integrate();
    void set_input(const std::vector<Scalar>& r, const std::vector<Scalar>& i);
    void set_feedback(const std::vector<Scalar>& r, const std::vector<Scalar>& i);
    void normalize();
};

class NawaSystem {
private:
    std::vector<std::unique_ptr<NawaSphere>> spheres;
    size_t global_tick = 0;
    size_t base_dim = 0;

public:
    NawaSystem(size_t dimension);
    void step(const std::vector<Scalar>& input_r, const std::vector<Scalar>& input_i);
    void save(const std::string& filename);
    void load(const std::string& filename);
    std::vector<Scalar> get_layer_r(int idx);
    std::vector<Scalar> get_layer_i(int idx);
    void learn(int layer_idx, 
               const std::vector<Scalar>& in_r, const std::vector<Scalar>& in_i,
               const std::vector<Scalar>& out_r, const std::vector<Scalar>& out_i);
};

