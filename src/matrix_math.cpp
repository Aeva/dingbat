
#include "matrix_math.h"
#include <sstream>
#include <string>
#include <string.h>
#include <glm.hpp>


using std::type_info;


PyTypeObject Vec2Type;
PyTypeObject Vec3Type;
PyTypeObject Vec4Type;
PyTypeObject Mat2Type;
PyTypeObject Mat3Type;
PyTypeObject Mat4Type;




template <typename T>
PyObject* TypeObjectNew(PyTypeObject* Type, PyObject* Args, PyObject* Kwargs)
{
    MathHandle<T>* Self;
    Self = (MathHandle<T>*)(Type->tp_alloc(Type, 0));
    return (PyObject*)Self;
}




template <typename T>
int TypeObjectInit(PyObject* Self, PyObject* Args, PyObject* Kwargs)
{
    //MathHandle<T>* Handle = (MathHandle<T>*)Self;
    // TODO : optional non-identity initialization paths based on type
    return 0;
}




template <typename T>
void TypeObjectDeAlloc(PyObject* Self)
{
    //PyObject_GC_UnTrack(Self); // <--- segfaults?
    //MathHandle<T>* Handle = (MathHandle<T>*)Self;
    Py_TYPE(Self)->tp_free(Self);
}




template <typename T>
bool InitMathType(PyObject* Module, const char* TypeName, PyTypeObject& TypeObject)
{
    TypeObject = {
	PyVarObject_HEAD_INIT(NULL, 0)
    };

    std::ostringstream NameStream;
    NameStream << "dingbat." << TypeName;
    std::string FullName = NameStream.str();
    
    TypeObject.tp_name = strdup(FullName.data());
    TypeObject.tp_basicsize = sizeof(T);
    TypeObject.tp_dealloc = (destructor)TypeObjectDeAlloc<T>;
    TypeObject.tp_flags = Py_TPFLAGS_DEFAULT;
    TypeObject.tp_doc = "";
    TypeObject.tp_new = TypeObjectNew<T>;
    TypeObject.tp_init = (initproc)TypeObjectInit<T>;

    if (PyType_Ready(&TypeObject) < 0)
    {
        return false;
    }
    Py_INCREF(&TypeObject);
    PyModule_AddObject(Module, TypeName, (PyObject*)&TypeObject);
    
    return true;
}




bool InitMathTypes(PyObject* Module)
{
    return (InitMathType<glm::vec2>(Module, "vec2", Vec2Type) &&
	    InitMathType<glm::vec3>(Module, "vec3", Vec3Type) &&
	    InitMathType<glm::vec4>(Module, "vec4", Vec4Type) &&
	    InitMathType<glm::mat2>(Module, "mat2", Mat2Type) &&
	    InitMathType<glm::mat3>(Module, "mat3", Mat3Type) &&
	    InitMathType<glm::mat4>(Module, "mat4", Mat4Type));
}
