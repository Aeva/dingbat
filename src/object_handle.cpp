
#include "object_handle.h"
#include <structmember.h>
#include <functional>
#include <iostream>

using std::function;


PyTypeObject ObjectHandleType;


struct ObjectHandle
{
    PyObject_HEAD
    void* Wrapped;
    function<void(void*)> Deleter;
};




PyObject* ObjectHandleNew(PyTypeObject* Type, PyObject* Args, PyObject* Kwargs)
{
    ObjectHandle* Self;
    Self = (ObjectHandle*)(Type->tp_alloc(Type, 0));
    Self->Wrapped = nullptr;
    Self->Deleter = [](void* Wrapped) {};
    return (PyObject*)Self;
}




int ObjectHandleInit(PyObject* Self, PyObject* Args, PyObject* Kwargs)
{
    //ObjectHandle* Handle = (ObjectHandle*)Self;
    return 0;
}




void ObjectHandleDeAlloc(PyObject* Self)
{
    //PyObject_GC_UnTrack(Self); // <--- segfaults?
    ObjectHandle* Handle = (ObjectHandle*)Self;
    if (Handle->Wrapped)
    {
	Handle->Deleter(Handle->Wrapped);
	Handle->Wrapped = nullptr;
    }
    Py_TYPE(Self)->tp_free(Self);
}




bool InitObjectHandleType(PyObject* Module)
{
    ObjectHandleType = {
	PyVarObject_HEAD_INIT(NULL, 0)
    };
    ObjectHandleType.tp_name = "dingbat.handle";
    ObjectHandleType.tp_basicsize = sizeof(ObjectHandle);
    ObjectHandleType.tp_dealloc = (destructor)ObjectHandleDeAlloc;
    ObjectHandleType.tp_flags = Py_TPFLAGS_DEFAULT;
    ObjectHandleType.tp_doc = "";
    ObjectHandleType.tp_new = ObjectHandleNew;
    ObjectHandleType.tp_init = (initproc)ObjectHandleInit;

    if (PyType_Ready(&ObjectHandleType) < 0)
    {
        return false;
    }
    Py_INCREF(&ObjectHandleType);
    PyModule_AddObject(Module, "handle", (PyObject*)&ObjectHandleType);
    
    return true;
}




template<typename T>
PyObject* WrapObject(shared_ptr<T> ManagedObject)
{
    PyObject *Args = PyTuple_New(0);
    PyObject *Initialized = PyObject_CallObject((PyObject*)&ObjectHandleType, Args);
    ObjectHandle* Handle = (ObjectHandle*)Initialized;
    Handle->Wrapped = (void*) new shared_ptr<T>(ManagedObject);
    Handle->Deleter = [](void* Wrapped) mutable
    {
	delete (shared_ptr<T>*) Wrapped;
    };
    Py_DECREF(Args);
    return Initialized;
}




template<typename T>
shared_ptr<T> AccessObject(PyObject* UserHandle)
{
    ObjectHandle* Handle = (ObjectHandle*)UserHandle;
    return *((shared_ptr<T>*)Handle->Wrapped);
}




class BasicManagedObject
{
public:
    BasicManagedObject()
    {
	std::cout << "Test Managed Object Created\n";
    }
    ~BasicManagedObject()
    {
	std::cout << "Test Managed Object Destroyed\n";
    }
};




PYTHON_API(CreateTestHandle)
{
    return WrapObject<BasicManagedObject>(std::make_shared<BasicManagedObject>());
}
