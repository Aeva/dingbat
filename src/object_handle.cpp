
#include "object_handle.h"
#include <structmember.h>
#include <iostream>


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
    Self->AccessorHook = []() -> void* { return nullptr; };
    Self->DeleterHook = []() -> void {};
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
    Handle->DeleterHook();
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




PyObject* CreateHandleObject(AccessorFunction AccessorHook, DeleterFunction DeleterHook)
{
    PyObject *Args = PyTuple_New(0);
    PyObject *Initialized = PyObject_CallObject((PyObject*)&ObjectHandleType, Args);
    ObjectHandle* Handle = (ObjectHandle*)Initialized;
    Handle->AccessorHook = AccessorHook;
    Handle->DeleterHook = DeleterHook;
    Py_DECREF(Args);
    return Initialized;
}




template<typename T>
PyObject* CreateHandleObject(shared_ptr<T> ManagedObject)
{
    weak_ptr<T> Weakref(ManagedObject);
    AccessorFunction Accessor = [Weakref]() -> void*
    {
	return (void*)&Weakref;
    };
    DeleterFunction Deleter = [ManagedObject]() mutable -> void
    {
	ManagedObject.reset();
    };
    return CreateHandleObject(Accessor, Deleter);
}




template<typename T>
weak_ptr<T> GetWrappedObject(PyObject* Self)
{
    ObjectHandle* Handle = (ObjectHandle*) Self;
    return *((weak_ptr<T>*)Handle->AccessorHook());
}




PYTHON_API(CreateTestHandle)
{
    AccessorFunction TestAccessor = []() -> void*
    {
	std::cout << "Test accessor function called!\n";
	return nullptr;
    };
    DeleterFunction TestDeleter = []()
    {
	std::cout << "Test deleter function called!\n";
    };
    return CreateHandleObject(TestAccessor, TestDeleter);
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



    
PYTHON_API(ManagedObjectTest)
{
    auto Pointer = std::make_shared<BasicManagedObject>();
    return CreateHandleObject<BasicManagedObject>(Pointer);
}
