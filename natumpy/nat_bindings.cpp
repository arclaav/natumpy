#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "nat_engine.hpp"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

typedef struct {
    PyObject_HEAD
    NawaEngine* engine;
} PyNawa;

static void PyNawa_dealloc(PyNawa* self) {
    if (self->engine) {
        delete self->engine;
    }
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* PyNawa_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    PyNawa* self;
    self = (PyNawa*)type->tp_alloc(type, 0);
    if (self != NULL) {
        int size = 100;
        if (!PyArg_ParseTuple(args, "|i", &size)) {
            Py_DECREF(self);
            return NULL;
        }
        self->engine = new NawaEngine((size_t)size);
    }
    return (PyObject*)self;
}

static PyObject* PyNawa_step(PyNawa* self, PyObject* args) {
    int cycles;
    if (!PyArg_ParseTuple(args, "i", &cycles)) return NULL;
    
    Py_BEGIN_ALLOW_THREADS
    self->engine->step(cycles);
    Py_END_ALLOW_THREADS
    
    Py_RETURN_NONE;
}

static PyObject* PyNawa_connect(PyNawa* self, PyObject* args) {
    int u, v, type;
    double w;
    if (!PyArg_ParseTuple(args, "iidi", &u, &v, &w, &type)) return NULL;
    
    self->engine->connect(u, v, w, type);
    Py_RETURN_NONE;
}

static PyObject* get_array_view(NawaEngine* engine, std::vector<double>& vec) {
    npy_intp dims[1] = { (npy_intp)vec.size() };
    return PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, vec.data());
}

static PyObject* PyNawa_get_qr(PyNawa* self, void* closure) {
    return get_array_view(self->engine, self->engine->state.q_r);
}

static PyObject* PyNawa_get_qi(PyNawa* self, void* closure) {
    return get_array_view(self->engine, self->engine->state.q_i);
}

static PyObject* PyNawa_get_pr(PyNawa* self, void* closure) {
    return get_array_view(self->engine, self->engine->state.p_r);
}

static PyObject* PyNawa_get_pi(PyNawa* self, void* closure) {
    return get_array_view(self->engine, self->engine->state.p_i);
}

static PyMethodDef PyNawa_methods[] = {
    {"step", (PyCFunction)PyNawa_step, METH_VARARGS, ""},
    {"connect", (PyCFunction)PyNawa_connect, METH_VARARGS, ""},
    {NULL}
};

static PyGetSetDef PyNawa_getset[] = {
    {"q_r", (getter)PyNawa_get_qr, NULL, "", NULL},
    {"q_i", (getter)PyNawa_get_qi, NULL, "", NULL},
    {"p_r", (getter)PyNawa_get_pr, NULL, "", NULL},
    {"p_i", (getter)PyNawa_get_pi, NULL, "", NULL},
    {NULL}
};

static PyTypeObject PyNawaType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "natcore.NawaEngine",
    .tp_basicsize = sizeof(PyNawa),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor)PyNawa_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_methods = PyNawa_methods,
    .tp_getset = PyNawa_getset,
    .tp_new = PyNawa_new,
};

static struct PyModuleDef natcoremodule = {
    PyModuleDef_HEAD_INIT,
    "natcore",
    NULL,
    -1,
    NULL
};

PyMODINIT_FUNC PyInit_natcore(void) {
    import_array();
    PyObject* m;
    
    if (PyType_Ready(&PyNawaType) < 0) return NULL;

    m = PyModule_Create(&natcoremodule);
    if (m == NULL) return NULL;

    Py_INCREF(&PyNawaType);
    if (PyModule_AddObject(m, "NawaEngine", (PyObject*)&PyNawaType) < 0) {
        Py_DECREF(&PyNawaType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}

