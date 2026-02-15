#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "nat_engine.hpp"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

typedef struct {
    PyObject_HEAD
    NawaSystem* sys;
} PyNawaSystem;

static void PyNawaSystem_dealloc(PyNawaSystem* self) {
    if (self->sys) delete self->sys;
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* PyNawaSystem_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    PyNawaSystem* self = (PyNawaSystem*)type->tp_alloc(type, 0);
    int dim = 512;
    if (self && PyArg_ParseTuple(args, "|i", &dim)) {
        self->sys = new NawaSystem((size_t)dim);
    }
    return (PyObject*)self;
}

static bool py_to_vector(PyObject* obj, std::vector<Scalar>& vec) {
    PyObject* arr = PyArray_ContiguousFromObject(obj, NPY_DOUBLE, 1, 1);
    if (!arr) return false;
    double* data = (double*)PyArray_DATA((PyArrayObject*)arr);
    vec.assign(data, data + PyArray_DIM((PyArrayObject*)arr, 0));
    Py_DECREF(arr);
    return true;
}

static PyObject* PyNawaSystem_step(PyNawaSystem* self, PyObject* args) {
    PyObject *ir, *ii;
    if (!PyArg_ParseTuple(args, "OO", &ir, &ii)) return NULL;
    std::vector<Scalar> vr, vi;
    if (!py_to_vector(ir, vr) || !py_to_vector(ii, vi)) return NULL;
    
    self->sys->step(vr, vi);
    Py_RETURN_NONE;
}

static PyObject* PyNawaSystem_save(PyNawaSystem* self, PyObject* args) {
    const char* f; if(!PyArg_ParseTuple(args, "s", &f)) return NULL;
    self->sys->save(std::string(f)); Py_RETURN_NONE;
}

static PyObject* PyNawaSystem_load(PyNawaSystem* self, PyObject* args) {
    const char* f; if(!PyArg_ParseTuple(args, "s", &f)) return NULL;
    self->sys->load(std::string(f)); Py_RETURN_NONE;
}

static PyObject* PyNawaSystem_get_state(PyNawaSystem* self, PyObject* args) {
    int layer;
    if (!PyArg_ParseTuple(args, "i", &layer)) return NULL;
    
    auto r = self->sys->get_layer_r(layer);
    auto i = self->sys->get_layer_i(layer);
    
    npy_intp dims[1] = { (npy_intp)r.size() };
    PyObject* ar = PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, r.data()); 
    PyObject* ai = PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, i.data());
    
    PyObject* safe_r = PyArray_NewCopy((PyArrayObject*)ar, NPY_ANYORDER);
    PyObject* safe_i = PyArray_NewCopy((PyArrayObject*)ai, NPY_ANYORDER);
    Py_DECREF(ar); Py_DECREF(ai);
    
    return Py_BuildValue("(OO)", safe_r, safe_i);
}

static PyObject* PyNawaSystem_learn(PyNawaSystem* self, PyObject* args) {
    int layer;
    PyObject *ir, *ii, *or_, *oi;
    if (!PyArg_ParseTuple(args, "iOOOO", &layer, &ir, &ii, &or_, &oi)) return NULL;
    std::vector<Scalar> vir, vii, vor, voi;
    if(!py_to_vector(ir, vir) || !py_to_vector(ii, vii) || !py_to_vector(or_, vor) || !py_to_vector(oi, voi)) return NULL;
    
    self->sys->learn(layer, vir, vii, vor, voi);
    
    Py_RETURN_NONE;
}

static PyMethodDef PyNawaSystem_methods[] = {
    {"step", (PyCFunction)PyNawaSystem_step, METH_VARARGS, ""},
    {"save", (PyCFunction)PyNawaSystem_save, METH_VARARGS, ""},
    {"load", (PyCFunction)PyNawaSystem_load, METH_VARARGS, ""},
    {"get_state", (PyCFunction)PyNawaSystem_get_state, METH_VARARGS, ""},
    {"learn", (PyCFunction)PyNawaSystem_learn, METH_VARARGS, ""},
    {NULL}
};

static PyTypeObject PyNawaSystemType = {
    PyVarObject_HEAD_INIT(NULL, 0) 
    .tp_name = "natcore.NawaSystem", 
    .tp_basicsize = sizeof(PyNawaSystem),
    .tp_dealloc = (destructor)PyNawaSystem_dealloc, 
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = PyNawaSystem_methods, 
    .tp_new = PyNawaSystem_new,
};

static struct PyModuleDef natcoremodule = { PyModuleDef_HEAD_INIT, "natcore", "Nawa Harmonic Engine", -1, NULL };
PyMODINIT_FUNC PyInit_natcore(void) {
    import_array();
    if (PyType_Ready(&PyNawaSystemType) < 0) return NULL;
    PyObject* m = PyModule_Create(&natcoremodule);
    if (!m) return NULL;
    Py_INCREF(&PyNawaSystemType);
    PyModule_AddObject(m, "NawaSystem", (PyObject*)&PyNawaSystemType);
    return m;
}

