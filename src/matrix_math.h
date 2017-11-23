#pragma once
#include "python_api.h"
#include <memory>

using std::shared_ptr;
using std::make_shared;

#define MATH_BINDINGS \
    {"perspective_matrix", (PyCFunction)PerspectiveMatrix, METH_FASTCALL, NULL}, \
    {"orthographic_matrix", (PyCFunction)OrthographicMatrix, METH_FASTCALL, NULL}, \
    {"lookat_matrix", (PyCFunction)LookAtMatrix, METH_FASTCALL, NULL}, \
    {"translation_matrix", (PyCFunction)TranslationMatrix, METH_FASTCALL, NULL}, \
    {"rotation_matrix", (PyCFunction)RotationMatrix, METH_FASTCALL, NULL}, \
    {"scale_matrix", (PyCFunction)ScaleMatrix, METH_FASTCALL, NULL}, \
    {"multiply_matrices", (PyCFunction)MultiplyMatrices, METH_FASTCALL, NULL},

template<typename T>
struct MathHandle
{
    PyObject_HEAD
    T Wrapped;
};




bool InitMathTypes(PyObject* Module);
PYTHON_API(PerspectiveMatrix);
PYTHON_API(OrthographicMatrix);
PYTHON_API(LookAtMatrix);
PYTHON_API(TranslationMatrix);
PYTHON_API(RotationMatrix);
PYTHON_API(ScaleMatrix);
PYTHON_API(MultiplyMatrices);
