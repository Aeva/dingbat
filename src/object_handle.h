#pragma once

#include "python_api.h"
#include <functional>
#include <memory>

using std::shared_ptr;
using std::make_shared;
using std::function;


struct ObjectHandle
{
    PyObject_HEAD
    void* Wrapped;
    function<void(void*)> Deleter;
};




bool InitObjectHandleType(PyObject* Module);
PyObject* NewHandle(void* Wrapped, function<void(void*)>& Deleter);
PYTHON_API(CreateTestHandle);




template<typename T>
PyObject* WrapObject(shared_ptr<T> ManagedObject)
{
    void* Wrapped = (void*) new shared_ptr<T>(ManagedObject);
    function<void(void*)> Deleter = [](void* Wrapped) mutable
    {
	delete (shared_ptr<T>*) Wrapped;
    };
    return NewHandle(Wrapped, Deleter);
}




template<typename T>
PyObject* WrapLambda(auto&& Lambda)
{
    return WrapObject<T>(make_shared<T>(Lambda));
}




template<typename T>
inline shared_ptr<T> AccessObject(PyObject* UserHandle)
{
    ObjectHandle* Handle = (ObjectHandle*)UserHandle;
    return *((shared_ptr<T>*)Handle->Wrapped);
}
