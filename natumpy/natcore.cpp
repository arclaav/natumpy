#include <Python.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>

struct Entity {
    double mu;
    double sigma;
    double entropy;
    double stability;
    long long id;
};

class Substrate {
public:
    std::vector<Entity> population;
    double global_temperature;
    double entropy_rate;
    long long next_id;
    std::mt19937 gen;

    Substrate() : global_temperature(1.0), entropy_rate(0.05), next_id(0) {
        std::random_device rd;
        gen = std::mt19937(rd());
    }

    void inject(double mu, double sigma) {
        Entity e;
        e.mu = mu;
        e.sigma = sigma;
        e.entropy = 0.0;
        e.stability = 0.5;
        e.id = next_id++;
        population.push_back(e);
    }

    void cycle() {
        if (population.empty()) return;

        std::uniform_int_distribution<> dis(0, population.size() - 1);
        std::normal_distribution<> noise(0.0, global_temperature * 0.1);

        for (auto& e : population) {
            e.entropy += entropy_rate;
            e.sigma += std::abs(noise(gen));
            
            if (e.stability > 0.0) e.stability -= 0.01;
        }

        size_t interaction_count = population.size() / 2;
        for (size_t i = 0; i < interaction_count; ++i) {
            int idx_a = dis(gen);
            int idx_b = dis(gen);

            if (idx_a == idx_b) continue;

            Entity& a = population[idx_a];
            Entity& b = population[idx_b];

            double delta = std::abs(a.mu - b.mu);
            double resonance = 1.0 / (1.0 + delta);

            if (resonance > 0.8) {
                double total_weight = (1.0/a.sigma) + (1.0/b.sigma);
                double new_mu = (a.mu/a.sigma + b.mu/b.sigma) / total_weight;
                double new_sigma = 1.0 / sqrt(total_weight);

                a.mu = new_mu;
                a.sigma = new_sigma;
                a.entropy = 0.0; 
                a.stability += 0.1;

                b.entropy += 0.5; 
            } else {
                a.entropy += 0.1;
                b.entropy += 0.1;
                a.sigma *= 1.1;
                b.sigma *= 1.1;
            }
        }

        population.erase(
            std::remove_if(population.begin(), population.end(),
                [](const Entity& e) { return e.entropy > 1.0; }),
            population.end()
        );
    }

    PyObject* snapshot() {
        PyObject* list = PyList_New(population.size());
        for (size_t i = 0; i < population.size(); ++i) {
            PyObject* dict = PyDict_New();
            PyDict_SetItemString(dict, "id", PyLong_FromLongLong(population[i].id));
            PyDict_SetItemString(dict, "mu", PyFloat_FromDouble(population[i].mu));
            PyDict_SetItemString(dict, "sigma", PyFloat_FromDouble(population[i].sigma));
            PyDict_SetItemString(dict, "entropy", PyFloat_FromDouble(population[i].entropy));
            PyDict_SetItemString(dict, "stability", PyFloat_FromDouble(population[i].stability));
            PyList_SetItem(list, i, dict);
        }
        return list;
    }
};

static void destroy_substrate(PyObject* cap) {
    Substrate* s = (Substrate*)PyCapsule_GetPointer(cap, "NatSubstrate");
    delete s;
}

static PyObject* create_system(PyObject* self, PyObject* args) {
    Substrate* s = new Substrate();
    return PyCapsule_New(s, "NatSubstrate", destroy_substrate);
}

static PyObject* inject_data(PyObject* self, PyObject* args) {
    PyObject* cap;
    double mu, sigma;
    if (!PyArg_ParseTuple(args, "Odd", &cap, &mu, &sigma)) return NULL;
    Substrate* s = (Substrate*)PyCapsule_GetPointer(cap, "NatSubstrate");
    s->inject(mu, sigma);
    Py_RETURN_NONE;
}

static PyObject* run_cycle(PyObject* self, PyObject* args) {
    PyObject* cap;
    int steps;
    if (!PyArg_ParseTuple(args, "Oi", &cap, &steps)) return NULL;
    Substrate* s = (Substrate*)PyCapsule_GetPointer(cap, "NatSubstrate");
    for(int i=0; i<steps; ++i) s->cycle();
    Py_RETURN_NONE;
}

static PyObject* get_state(PyObject* self, PyObject* args) {
    PyObject* cap;
    if (!PyArg_ParseTuple(args, "O", &cap)) return NULL;
    Substrate* s = (Substrate*)PyCapsule_GetPointer(cap, "NatSubstrate");
    return s->snapshot();
}

static PyMethodDef NatCoreMethods[] = {
    {"create_system", create_system, METH_VARARGS, ""},
    {"inject_data", inject_data, METH_VARARGS, ""},
    {"run_cycle", run_cycle, METH_VARARGS, ""},
    {"get_state", get_state, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef natcoremodule = {
    PyModuleDef_HEAD_INIT, "natcore", NULL, -1, NatCoreMethods
};

PyMODINIT_FUNC PyInit_natcore(void) {
    return PyModule_Create(&natcoremodule);
}

