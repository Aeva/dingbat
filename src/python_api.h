#pragma once

#include <Python.h>


#define PYTHON_API(name) PyObject* name (PyObject *module, PyObject **args, Py_ssize_t nargs, PyObject *kwnames)
