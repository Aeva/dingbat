#pragma once

#include "python_api.h"
#include <functional>
#include <memory>

using std::function;
using std::shared_ptr;
using std::weak_ptr;

using AccessorFunction = function<void*()>;
using DeleterFunction = function<void()>;


bool InitObjectHandleType(PyObject* Module);
PyObject* CreateHandleObject(AccessorFunction AccessorHook, DeleterFunction DeleteHook);
template<typename T> PyObject* CreateHandleObject(shared_ptr<T> ManagedObject);
template<typename T> weak_ptr<T> GetWrappedObject(PyObject* Handle);
PYTHON_API(CreateTestHandle);
PYTHON_API(ManagedObjectTest);

