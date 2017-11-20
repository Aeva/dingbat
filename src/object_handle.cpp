
#include "object_handle.h"
#include <structmember.h>
#include <functional>
#include <iostream>

using std::function;


PyTypeObject ObjectHandleType;


struct ObjectHandle
{
    PyObject_HEAD
    AccessorFunction AccessorHook;
    DeleterFunction DeleterHook;
};




PyObject* ObjectHandleNew(PyTypeObject* Type, PyObject* Args, PyObject* Kwargs)
{
    ObjectHandle* Self;
    Self = (ObjectHandle*)(Type->tp_alloc(Type, 0));
    Self->AccessorHook = nullptr;
    Self->DeleterHook = nullptr;
    return (PyObject*)Self;
}




void* TestAccessor() { return nullptr; }
void TestDeleter() { std::cout << "DEBUG : Deleter handle called.\n"; }


int ObjectHandleInit(PyObject* Self, PyObject* Args, PyObject* Kwargs)
{
    ObjectHandle* Handle = (ObjectHandle*)Self;
    Handle->AccessorHook = TestAccessor;
    Handle->DeleterHook = TestDeleter;
    return 0;
}




void ObjectHandleDeAlloc(PyObject* Self)
{
    //PyObject_GC_UnTrack(Self); // <--- segfaults?
    ObjectHandle* Handle = (ObjectHandle*)Self;
    if (Handle->DeleterHook)
    {
	Handle->DeleterHook();
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




PyObject* CreateHandleObject(AccessorFunction AccessorHook, DeleterFunction DeleteHook)
{
    PyObject *Args = PyTuple_New(0);
    PyObject *Initialized = PyObject_CallObject((PyObject*)&ObjectHandleType, Args);
    Py_DECREF(Args);
    return Initialized;
}



    
PYTHON_API(CreateTestHandle)
{
    return CreateHandleObject(nullptr, nullptr);
}
