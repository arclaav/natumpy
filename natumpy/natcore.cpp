#include <Python.h>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <random>

struct NatPotential {
    std::vector<double> realities;
    std::vector<std::string> origins; 
    double instability_index;
};

static std::random_device rd;
static std::mt19937 gen(rd());

static PyObject* create_chaos(PyObject* self, PyObject* args) {
    double value;
    if (!PyArg_ParseTuple(args, "d", &value)) {
        return NULL;
    }
    
    NatPotential* p = new NatPotential();
    p->realities.push_back(value);
    p->origins.push_back("genesis");
    p->instability_index = 0.0;

    return PyCapsule_New(p, "NatPotential", [](PyObject* cap) {
        NatPotential* ptr = (NatPotential*)PyCapsule_GetPointer(cap, "NatPotential");
        delete ptr;
    });
}

static PyObject* provoke(PyObject* self, PyObject* args) {
    PyObject *cap_a, *cap_b;
    if (!PyArg_ParseTuple(args, "OO", &cap_a, &cap_b)) {
        return NULL;
    }

    NatPotential* a = (NatPotential*)PyCapsule_GetPointer(cap_a, "NatPotential");
    NatPotential* b = (NatPotential*)PyCapsule_GetPointer(cap_b, "NatPotential");

    NatPotential* res = new NatPotential();
    
    for (double val_a : a->realities) {
        for (double val_b : b->realities) {
            
            res->realities.push_back(val_a + val_b);
            res->origins.push_back("additive_harmony");

            res->realities.push_back(val_a - val_b);
            res->origins.push_back("subtractive_conflict");

            res->realities.push_back(val_a * val_b);
            res->origins.push_back("multiplicative_synergy");

            if (std::abs(val_b) > 1e-9) {
                res->realities.push_back(val_a / val_b);
                res->origins.push_back("divisive_split");
            } else {
                res->realities.push_back(INFINITY);
                res->origins.push_back("singularity");
            }
        }
    }

    res->instability_index = a->instability_index + b->instability_index + 1.0;

    return PyCapsule_New(res, "NatPotential", [](PyObject* cap) {
        NatPotential* ptr = (NatPotential*)PyCapsule_GetPointer(cap, "NatPotential");
        delete ptr;
    });
}

static PyObject* witness(PyObject* self, PyObject* args) {
    PyObject* cap;
    if (!PyArg_ParseTuple(args, "O", &cap)) {
        return NULL;
    }
    NatPotential* p = (NatPotential*)PyCapsule_GetPointer(cap, "NatPotential");
    
    PyObject* py_realities = PyList_New(p->realities.size());
    PyObject* py_origins = PyList_New(p->origins.size());

    for (size_t i = 0; i < p->realities.size(); ++i) {
        PyList_SetItem(py_realities, i, PyFloat_FromDouble(p->realities[i]));
        PyList_SetItem(py_origins, i, PyUnicode_FromString(p->origins[i].c_str()));
    }

    return Py_BuildValue("{s:O,s:O,s:d}", 
        "potentials", py_realities, 
        "narratives", py_origins,
        "chaos_level", p->instability_index);
}

static PyObject* force_choice(PyObject* self, PyObject* args) {
    PyObject* cap;
    int strategy; 
    if (!PyArg_ParseTuple(args, "Oi", &cap, &strategy)) {
        return NULL;
    }
    NatPotential* p = (NatPotential*)PyCapsule_GetPointer(cap, "NatPotential");
    
    if (p->realities.empty()) return Py_BuildValue("d", 0.0);

    double chosen_one = 0.0;
    
    if (strategy == 0) { 
        double sum = 0;
        for(double v : p->realities) if(!std::isinf(v)) sum += v;
        chosen_one = sum / p->realities.size();
    } else if (strategy == 1) { 
        double max_v = -INFINITY;
        for(double v : p->realities) if(v > max_v) max_v = v;
        chosen_one = max_v;
    } else { 
        std::uniform_int_distribution<> dis(0, p->realities.size() - 1);
        chosen_one = p->realities[dis(gen)];
    }

    return Py_BuildValue("d", chosen_one);
}

static PyMethodDef NatCoreMethods[] = {
    {"create_chaos", create_chaos, METH_VARARGS, ""},
    {"provoke", provoke, METH_VARARGS, ""},
    {"witness", witness, METH_VARARGS, ""},
    {"force_choice", force_choice, METH_VARARGS, ""},
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
