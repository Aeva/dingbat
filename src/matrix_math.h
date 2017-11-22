#pragma once
#include "python_api.h"
#include <memory>

using std::shared_ptr;
using std::make_shared;

#define MATH_BINDINGS \
    

template<typename T>
struct MathHandle
{
    PyObject_HEAD
    T Wrapped;
};




bool InitMathTypes(PyObject* Module);
