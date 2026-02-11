#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <new>
#include <mutex>

struct NatAtom {
    double value;
    double velocity;
    double mass;
    double damping;
    double target;
};

static_assert(sizeof(NatAtom) == 40, "Size mismatch");

class NatFieldEngine {
public:
    std::vector<NatAtom> atoms;
    std::mutex mtx;

    NatFieldEngine(size_t size) {
        if (size < 1) size = 1;
        atoms.resize(size);
        for(auto& a : atoms) {
            a.value = 0.0;
            a.velocity = 0.0;
            a.mass = 1.0;
            a.damping = 0.1;
            a.target = 0.0;
        }
    }

    void set_properties(size_t idx, double m, double d) {
        std::lock_guard<std::mutex> lock(mtx);
        if (idx < atoms.size()) {
            if (m < 1e-6) m = 1e-6;
            if (d < 0.0) d = 0.0;
            if (d > 1.0) d = 1.0;
            atoms[idx].mass = m;
            atoms[idx].damping = d;
        }
    }

    void set_target(size_t idx, double t) {
        std::lock_guard<std::mutex> lock(mtx);
        if (idx < atoms.size()) {
            if (std::isfinite(t)) {
                atoms[idx].target = t;
            } else {
                atoms[idx].target = 0.0;
            }
        }
    }

    void step_physics(int cycles, double dt) {
        std::lock_guard<std::mutex> lock(mtx);
        if (dt <= 0.0) dt = 0.01;
        if (cycles < 1) return;
        
        for (int k = 0; k < cycles; ++k) {
            for (auto& a : atoms) {
                double force = (a.target - a.value);
                double acceleration = force / a.mass;
                
                a.velocity += acceleration * dt;
                a.velocity *= (1.0 - a.damping);
                a.value += a.velocity * dt;

                if (!std::isfinite(a.value)) {
                    a.value = 0.0;
                    a.velocity = 0.0;
                }
            }
        }
    }

    void resonate(size_t idx_a, size_t idx_b, double coupling_strength) {
        std::lock_guard<std::mutex> lock(mtx);
        if (idx_a >= atoms.size() || idx_b >= atoms.size()) return;
        if (idx_a == idx_b) return;

        NatAtom& a = atoms[idx_a];
        NatAtom& b = atoms[idx_b];

        double delta = b.value - a.value;
        double force = delta * coupling_strength;

        a.velocity += (force / a.mass) * 0.1;
        b.velocity -= (force / b.mass) * 0.1;
    }

    void force_state(size_t idx, double v) {
        std::lock_guard<std::mutex> lock(mtx);
        if (idx < atoms.size()) {
             if (std::isfinite(v)) {
                atoms[idx].value = v;
                atoms[idx].velocity = 0.0;
             }
        }
    }
};

typedef struct {
    PyObject_HEAD
    NatFieldEngine* engine;
    Py_ssize_t shape[1];
    Py_ssize_t strides[1];
} PyNatField;

static void NatField_dealloc(PyNatField* self) {
    if (self->engine) {
        delete self->engine;
    }
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* NatField_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    PyNatField* self = (PyNatField*)type->tp_alloc(type, 0);
    int size = 100;
    if (self != NULL) {
        if (!PyArg_ParseTuple(args, "|i", &size)) return NULL;
        self->engine = new NatFieldEngine(size);
        self->shape[0] = self->engine->atoms.size();
        self->strides[0] = sizeof(NatAtom);
    }
    return (PyObject*)self;
}

static PyObject* NatField_set_props(PyNatField* self, PyObject* args) {
    int idx;
    double m, d;
    if (!PyArg_ParseTuple(args, "idd", &idx, &m, &d)) return NULL;
    self->engine->set_properties(idx, m, d);
    Py_RETURN_NONE;
}

static PyObject* NatField_set_target(PyNatField* self, PyObject* args) {
    int idx;
    double t;
    if (!PyArg_ParseTuple(args, "id", &idx, &t)) return NULL;
    self->engine->set_target(idx, t);
    Py_RETURN_NONE;
}

static PyObject* NatField_step(PyNatField* self, PyObject* args) {
    int cycles;
    double dt = 0.1;
    if (!PyArg_ParseTuple(args, "i|d", &cycles, &dt)) return NULL;
    
    Py_BEGIN_ALLOW_THREADS
    self->engine->step_physics(cycles, dt);
    Py_END_ALLOW_THREADS
    
    Py_RETURN_NONE;
}

static PyObject* NatField_resonate(PyNatField* self, PyObject* args) {
    int i1, i2;
    double strength;
    if (!PyArg_ParseTuple(args, "iid", &i1, &i2, &strength)) return NULL;
    self->engine->resonate(i1, i2, strength);
    Py_RETURN_NONE;
}

static PyObject* NatField_force(PyNatField* self, PyObject* args) {
    int idx;
    double v;
    if (!PyArg_ParseTuple(args, "id", &idx, &v)) return NULL;
    self->engine->force_state(idx, v);
    Py_RETURN_NONE;
}

static PyObject* NatField_size(PyNatField* self, PyObject* args) {
    if (self->engine == NULL) return PyLong_FromLong(0);
    return PyLong_FromSize_t(self->engine->atoms.size());
}

static int NatField_getbuffer(PyNatField *self, Py_buffer *view, int flags) {
    if (self->engine == NULL) return -1;
    
    view->obj = (PyObject*)self;
    Py_INCREF(view->obj);
    view->buf = (void*)self->engine->atoms.data();
    view->len = self->engine->atoms.size() * sizeof(NatAtom);
    view->readonly = 0;
    view->itemsize = sizeof(NatAtom);
    view->format = (char*)"ddddd";
    view->ndim = 1;
    view->shape = self->shape;
    view->strides = self->strides;
    view->suboffsets = NULL;
    view->internal = NULL;
    return 0;
}

static void NatField_releasebuffer(PyNatField *self, Py_buffer *view) {
}

static PyMethodDef NatField_methods[] = {
    {"set_props", (PyCFunction)NatField_set_props, METH_VARARGS, ""},
    {"set_target", (PyCFunction)NatField_set_target, METH_VARARGS, ""},
    {"step", (PyCFunction)NatField_step, METH_VARARGS, ""},
    {"resonate", (PyCFunction)NatField_resonate, METH_VARARGS, ""},
    {"force", (PyCFunction)NatField_force, METH_VARARGS, ""},
    {"size", (PyCFunction)NatField_size, METH_NOARGS, ""},
    {NULL}
};

static PyTypeObject PyNatFieldType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "natcore.NatField",
    sizeof(PyNatField),
    0,
    (destructor)NatField_dealloc,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    0, 0, 0, 0, 0, 0, 0,
    NatField_methods,
    0, 0, 0, 0, 0, 0, 0,
    0,
};

static struct PyModuleDef natcoremodule = {
    PyModuleDef_HEAD_INIT,
    "natcore",
    NULL,
    -1,
    NULL
};

PyMODINIT_FUNC PyInit_natcore(void) {
    PyObject* m;
    
    static PyBufferProcs buffer_procs;
    buffer_procs.bf_getbuffer = (getbufferproc)NatField_getbuffer;
    buffer_procs.bf_releasebuffer = (releasebufferproc)NatField_releasebuffer;
    PyNatFieldType.tp_as_buffer = &buffer_procs;
    
    PyNatFieldType.tp_new = (newfunc)NatField_new;

    if (PyType_Ready(&PyNatFieldType) < 0) return NULL;

    m = PyModule_Create(&natcoremodule);
    if (m == NULL) return NULL;

    Py_INCREF(&PyNatFieldType);
    if (PyModule_AddObject(m, "NatField", (PyObject*)&PyNatFieldType) < 0) {
        Py_DECREF(&PyNatFieldType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}

