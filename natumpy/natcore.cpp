#include <Python.h>
#include <cmath>
#include <random>
#include <vector>
#include <chrono>
#include <memory>

struct NatEntity {
    double mu;
    double sigma;
    double entropy;
    unsigned long long timestamp;
};

static std::random_device rd;
static std::mt19937 gen(rd());

unsigned long long current_time_micros() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

static PyObject* create_entity(PyObject* self, PyObject* args) {
    double mu, sigma;
    if (!PyArg_ParseTuple(args, "dd", &mu, &sigma)) {
        return NULL;
    }
    
    NatEntity* entity = new NatEntity();
    entity->mu = mu;
    entity->sigma = sigma;
    entity->entropy = 0.0;
    entity->timestamp = current_time_micros();

    return PyCapsule_New(entity, "NatEntity", [](PyObject* cap) {
        NatEntity* e = (NatEntity*)PyCapsule_GetPointer(cap, "NatEntity");
        delete e;
    });
}

static PyObject* entangle(PyObject* self, PyObject* args) {
    PyObject *cap_a, *cap_b;
    if (!PyArg_ParseTuple(args, "OO", &cap_a, &cap_b)) {
        return NULL;
    }

    NatEntity* a = (NatEntity*)PyCapsule_GetPointer(cap_a, "NatEntity");
    NatEntity* b = (NatEntity*)PyCapsule_GetPointer(cap_b, "NatEntity");

    if (!a || !b) return NULL;

    NatEntity* result = new NatEntity();
    
    result->mu = a->mu + b->mu;
    result->sigma = std::sqrt(std::pow(a->sigma, 2) + std::pow(b->sigma, 2));
    
    unsigned long long now = current_time_micros();
    double dt_a = (now - a->timestamp) / 1000000.0;
    double dt_b = (now - b->timestamp) / 1000000.0;
    
    result->entropy = (a->entropy + dt_a) + (b->entropy + dt_b);
    result->timestamp = now;

    return PyCapsule_New(result, "NatEntity", [](PyObject* cap) {
        NatEntity* e = (NatEntity*)PyCapsule_GetPointer(cap, "NatEntity");
        delete e;
    });
}

static PyObject* collapse(PyObject* self, PyObject* args) {
    PyObject* cap;
    if (!PyArg_ParseTuple(args, "O", &cap)) {
        return NULL;
    }

    NatEntity* e = (NatEntity*)PyCapsule_GetPointer(cap, "NatEntity");
    if (!e) return NULL;

    unsigned long long now = current_time_micros();
    double dt = (now - e->timestamp) / 1000000.0;
    
    double current_sigma = e->sigma + (dt * 0.1);

    std::normal_distribution<double> d(e->mu, current_sigma);
    double observed_value = d(gen);

    e->timestamp = now;
    e->sigma = current_sigma * 0.5;

    return Py_BuildValue("d", observed_value);
}

static PyObject* inspect(PyObject* self, PyObject* args) {
    PyObject* cap;
    if (!PyArg_ParseTuple(args, "O", &cap)) {
        return NULL;
    }
    NatEntity* e = (NatEntity*)PyCapsule_GetPointer(cap, "NatEntity");
    return Py_BuildValue("{s:d,s:d,s:d}", "mu", e->mu, "sigma", e->sigma, "entropy", e->entropy);
}

static PyMethodDef NatCoreMethods[] = {
    {"create_entity", create_entity, METH_VARARGS, ""},
    {"entangle", entangle, METH_VARARGS, ""},
    {"collapse", collapse, METH_VARARGS, ""},
    {"inspect", inspect, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef natcoremodule = {
    PyModuleDef_HEAD_INIT,
    "natcore",
    NULL,
    -1,
    NatCoreMethods
};

PyMODINIT_FUNC PyInit_natcore(void) {
    return PyModule_Create(&natcoremodule);
}

