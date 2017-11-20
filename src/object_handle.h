#pragma once

#include "python_api.h"
#include <memory>

using std::shared_ptr;


bool InitObjectHandleType(PyObject* Module);
template<typename T> PyObject* WrapObject(shared_ptr<T> ManagedObject);
template<typename T> shared_ptr<T> AccessObject(PyObject* Handle);
PYTHON_API(CreateTestHandle);

