#include "nat_engine.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <vector>
#include <random>
#include <iostream>
#include <fstream>

inline Scalar sigmoid(Scalar x) { return 1.0 / (1.0 + std::exp(-x)); }
inline Scalar tanh_activation(Scalar x) { return std::tanh(x); }

void NawaMemory::init(size_t dimension) {
    dim = dimension;
    W_recurrent_r.resize(dim * dim, 0.0); 
    W_recurrent_i.resize(dim * dim, 0.0);
    
    W_gate_r.resize(dim); W_gate_i.resize(dim);
    W_input_r.resize(dim); W_input_i.resize(dim);
    W_feedback_r.resize(dim); W_feedback_i.resize(dim);
}

void NawaMemory::save(std::ostream& out) {
    out.write((char*)&dim, sizeof(size_t));
    out.write((char*)W_recurrent_r.data(), dim*dim*sizeof(Scalar));
    out.write((char*)W_recurrent_i.data(), dim*dim*sizeof(Scalar));
    out.write((char*)W_gate_r.data(), dim*sizeof(Scalar)); out.write((char*)W_gate_i.data(), dim*sizeof(Scalar));
    out.write((char*)W_input_r.data(), dim*sizeof(Scalar)); out.write((char*)W_input_i.data(), dim*sizeof(Scalar));
    out.write((char*)W_feedback_r.data(), dim*sizeof(Scalar)); out.write((char*)W_feedback_i.data(), dim*sizeof(Scalar));
}

void NawaMemory::load(std::istream& in) {
    size_t loaded_dim;
    in.read((char*)&loaded_dim, sizeof(size_t));
    if(loaded_dim != dim) return;
    in.read((char*)W_recurrent_r.data(), dim*dim*sizeof(Scalar));
    in.read((char*)W_recurrent_i.data(), dim*dim*sizeof(Scalar));
    in.read((char*)W_gate_r.data(), dim*sizeof(Scalar)); in.read((char*)W_gate_i.data(), dim*sizeof(Scalar));
    in.read((char*)W_input_r.data(), dim*sizeof(Scalar)); in.read((char*)W_input_i.data(), dim*sizeof(Scalar));
    in.read((char*)W_feedback_r.data(), dim*sizeof(Scalar)); in.read((char*)W_feedback_i.data(), dim*sizeof(Scalar));
}

NawaSphere::NawaSphere(size_t dimension, int id, int ratio) {
    layer_id = id; time_ratio = ratio;
    state.resize(dimension); memory.init(dimension);
    buf_input_r.resize(dimension, 0.0); buf_input_i.resize(dimension, 0.0);
    buf_feedback_r.resize(dimension, 0.0); buf_feedback_i.resize(dimension, 0.0);
}

void NawaSphere::init_weights(int seed) {
    std::mt19937 gen(seed);
    std::normal_distribution<Scalar> d(0.0, 0.05);

    for(size_t i=0; i<state.size; ++i) {
        for(size_t j=0; j<state.size; ++j) {
            size_t idx = i * state.size + j;
            if (i == j) {
                memory.W_recurrent_r[idx] = 1.0;
                memory.W_recurrent_i[idx] = 0.0;
            } else {
                memory.W_recurrent_r[idx] = d(gen) * 0.1;
                memory.W_recurrent_i[idx] = d(gen) * 0.1;
            }
        }
    }

    for(size_t i=0; i<state.size; ++i) {
        memory.W_gate_r[i] = d(gen) + 1.0;
        memory.W_gate_i[i] = d(gen) * 0.05;
        
        memory.W_input_r[i] = 1.0 + d(gen)*0.1; 
        memory.W_input_i[i] = d(gen)*0.1;
        
        memory.W_feedback_r[i] = 0.3 + d(gen)*0.1; memory.W_feedback_i[i] = d(gen)*0.1;
    }
}

void NawaSphere::set_input(const std::vector<Scalar>& r, const std::vector<Scalar>& i) {
    if(r.size()==state.size) { buf_input_r=r; buf_input_i=i; }
}
void NawaSphere::set_feedback(const std::vector<Scalar>& r, const std::vector<Scalar>& i) {
    if(r.size()==state.size) { buf_feedback_r=r; buf_feedback_i=i; }
}

void NawaSphere::step_integrate() {
    size_t N = state.size;
    std::vector<Scalar> next_r(N), next_i(N);
    
    for(size_t i=0; i<N; ++i) {
        Scalar sum_r = 0.0;
        Scalar sum_i = 0.0;
        for(size_t j=0; j<N; ++j) {
            size_t idx = i * N + j;
            Scalar w_r = memory.W_recurrent_r[idx];
            Scalar w_i = memory.W_recurrent_i[idx];
            Scalar s_r = state.q_r[j];
            Scalar s_i = state.q_i[j];
            
            sum_r += (w_r * s_r - w_i * s_i);
            sum_i += (w_r * s_i + w_i * s_r);
        }
        
        Scalar in_r = buf_input_r[i]*memory.W_input_r[i] - buf_input_i[i]*memory.W_input_i[i];
        Scalar in_i = buf_input_r[i]*memory.W_input_i[i] + buf_input_i[i]*memory.W_input_r[i];
        
        Scalar fb_r = buf_feedback_r[i]*memory.W_feedback_r[i] - buf_feedback_i[i]*memory.W_feedback_i[i];
        Scalar fb_i = buf_feedback_r[i]*memory.W_feedback_i[i] + buf_feedback_i[i]*memory.W_feedback_r[i];
        
        Scalar total_r = sum_r + in_r + fb_r;
        Scalar total_i = sum_i + in_i + fb_i;
        
        Scalar gate = sigmoid((memory.W_gate_r[i]*total_r - memory.W_gate_i[i]*total_i));
        
        next_r[i] = (1.0 - gate) * state.q_r[i] + gate * total_r;
        next_i[i] = (1.0 - gate) * state.q_i[i] + gate * total_i;
    }
    
    state.q_r = next_r;
    state.q_i = next_i;
    
    normalize(); 
}

void NawaSphere::normalize() {
    for(size_t i=0; i<state.size; ++i) {
        Scalar mag = std::sqrt(state.q_r[i]*state.q_r[i] + state.q_i[i]*state.q_i[i]);
        if (mag > 1e-9) {
            Scalar new_mag = std::tanh(mag); 
            Scalar scale = new_mag / mag;
            state.q_r[i] *= scale;
            state.q_i[i] *= scale;
        } else {
            state.q_r[i] = 0.0; state.q_i[i] = 0.0;
        }
    }
}

NawaSystem::NawaSystem(size_t dimension) {
    base_dim = dimension;
    global_tick = 0;
    int ratio = 1;
    for(int i=0; i<NAWA_LAYERS; ++i) {
        spheres.push_back(std::make_unique<NawaSphere>(dimension, i, ratio));
        spheres.back()->init_weights(42+i);
        ratio *= 2; 
    }
}

void NawaSystem::step(const std::vector<Scalar>& ir, const std::vector<Scalar>& ii) {
    global_tick++;
    spheres[0]->set_input(ir, ii);
    for(int i=0; i<NAWA_LAYERS; ++i) {
        if(global_tick % spheres[i]->time_ratio == 0) {
            if(i > 0) spheres[i]->set_input(spheres[i-1]->state.q_r, spheres[i-1]->state.q_i);
            if(i < NAWA_LAYERS-1) spheres[i]->set_feedback(spheres[i+1]->state.q_r, spheres[i+1]->state.q_i);
            spheres[i]->step_integrate();
        }
    }
}

void NawaSystem::save(const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    if(out) for(auto& s : spheres) s->memory.save(out);
}
void NawaSystem::load(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if(in) for(auto& s : spheres) s->memory.load(in);
}
std::vector<Scalar> NawaSystem::get_layer_r(int idx) {
    if(idx>=0 && idx<NAWA_LAYERS) return spheres[idx]->state.q_r;
    return {};
}
std::vector<Scalar> NawaSystem::get_layer_i(int idx) {
    if(idx>=0 && idx<NAWA_LAYERS) return spheres[idx]->state.q_i;
    return {};
}

void NawaSystem::learn(int idx, 
                       const std::vector<Scalar>& in_r, const std::vector<Scalar>& in_i,
                       const std::vector<Scalar>& error_r, const std::vector<Scalar>& error_i) {
    if (idx < 0 || idx >= NAWA_LAYERS) return;
    
    NawaSphere* s = spheres[idx].get();
    size_t N = s->state.size;
    Scalar lr = 0.05; 
    
    for(size_t i=0; i<N; ++i) {
        Scalar err_real = error_r[i];
        Scalar err_imag = error_i[i];
        
        for(size_t j=0; j<N; ++j) {
            size_t widx = i * N + j;
            Scalar inp_real = in_r[j]; 
            Scalar inp_imag = in_i[j];
            
            Scalar grad_r = err_real * inp_real + err_imag * inp_imag;
            Scalar grad_i = err_imag * inp_real - err_real * inp_imag;
            
            s->memory.W_recurrent_r[widx] += lr * grad_r;
            s->memory.W_recurrent_i[widx] += lr * grad_i;
            
            s->memory.W_recurrent_r[widx] *= 0.9999; 
            s->memory.W_recurrent_i[widx] *= 0.9999;
        }

        Scalar grad_in_r = err_real * in_r[i] + err_imag * in_i[i];
        Scalar grad_in_i = err_imag * in_r[i] - err_real * in_i[i];
        
        s->memory.W_input_r[i] += lr * grad_in_r;
        s->memory.W_input_i[i] += lr * grad_in_i;
    }
}

