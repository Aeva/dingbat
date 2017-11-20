#pragma once

#include "python_api.h"
using AccessorFunction = void* (*)();
using DeleterFunction = void (*)();


bool InitObjectHandleType(PyObject* Module);
PyObject* CreateHandleObject(AccessorFunction AccessorHook, DeleterFunction DeleteHook);
PYTHON_API(CreateTestHandle);
