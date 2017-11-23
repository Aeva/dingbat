
#include "matrix_math.h"
#include <type_traits>
#include <sstream>
#include <string>
#include <string.h>
#include <iostream>
#include <glm.hpp>


using std::type_info;


PyTypeObject Vec2Type;
PyTypeObject Vec3Type;
PyTypeObject Vec4Type;
PyTypeObject Mat2Type;
PyTypeObject Mat3Type;
PyTypeObject Mat4Type;




template <typename T>
constexpr bool IsVector()
{
    return (std::is_same<T, glm::vec2>::value ||
	    std::is_same<T, glm::vec3>::value ||
	    std::is_same<T, glm::vec4>::value);
}




template <typename T>
constexpr bool IsMatrix()
{
    return (std::is_same<T, glm::mat2>::value ||
	    std::is_same<T, glm::mat3>::value ||
	    std::is_same<T, glm::mat4>::value);
}




template <typename T>
inline void FillVector(T Vector, float* Values)
{
    static_assert(IsVector<T>());
    for (int i=0; i<Vector.length(); i++)
    {
	Vector[i] = Values[i];
    }
}




template <typename T>
inline void FillMatrix(T Matrix, float* Values)
{
    static_assert(IsMatrix<T>());
    constexpr int Period = (std::is_same<T, glm::mat2>::value ? 2 :
			    std::is_same<T, glm::mat3>::value ? 3 : 4);
    for (int i=0; i<Matrix.length(); i++)
    {
	int y = i % Period;
	int x = i / Period;
	Matrix[x][y] = Values[i];
    }
}




// TODO : find a cleaner way to do this.  `if constexpr` would be nice.
#define PERMUTE(T) inline void FillThing( T Vector, float* Values ) { FillVector<decltype(Vector)>(Vector, Values); }
PERMUTE(glm::vec2)
PERMUTE(glm::vec3)
PERMUTE(glm::vec4)
#undef PERMUTE
#define PERMUTE(T) inline void FillThing( T Matrix, float* Values ) { FillMatrix<decltype(Matrix)>(Matrix, Values); }
PERMUTE(glm::mat2)
PERMUTE(glm::mat3)
PERMUTE(glm::mat4)
#undef PERMUTE




template <typename T>
PyObject* TypeObjectNew(PyTypeObject* Type, PyObject* Args, PyObject* Kwargs)
{
    MathHandle<T>* Self;
    Self = (MathHandle<T>*)(Type->tp_alloc(Type, 0));
    return (PyObject*)Self;
}




// TODO : Returning -1 causes segfaults?  What is the correct error
//        code?
template <typename T>
int TypeObjectInit(PyObject* Self, PyObject* Args, PyObject* Kwargs)
{
    const int ArgCount = PyTuple_Size(Args);
    if (ArgCount == 0)
    {
	return 0;
    }

    MathHandle<T>* Handle = (MathHandle<T>*)Self;
    T& Vector = Handle->Wrapped;
    const int Size = Vector.length();
    
    if (ArgCount == 1)
    {
	PyObject* BufferArg = nullptr;
	if (!PyArg_ParseTuple(Args, "O", &BufferArg))
	{
	    PyErr_SetString(PyExc_RuntimeError, "Could not read buffer from argument list.");
	    return -1;
	}
	Py_buffer DataView;
	if (PyObject_GetBuffer(BufferArg, &DataView, PyBUF_CONTIG_RO) < 0)
	{
	    PyErr_SetString(PyExc_RuntimeError, "Could not access buffer view?");
	    return -1;
	}
	const int ItemCount = DataView.len / DataView.itemsize;
	if (Size == ItemCount)
	{
	    FillThing(Vector, (float*)DataView.buf);
	    PyBuffer_Release(&DataView);
	    return 0;
	}
	else
	{
	    PyErr_SetString(PyExc_RuntimeError, "Length mismatch between given buffer and target vector size.");
	    return -1;
	}
    }
    else
    {
	PyErr_SetString(PyExc_RuntimeError, "Unsupported number of arguments.");
	return -1;
    }
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
