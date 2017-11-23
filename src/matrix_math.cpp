
#include "matrix_math.h"
#include <type_traits>
#include <sstream>
#include <string>
#include <string.h>
#include <iostream>
#include <glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


using std::string;


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
inline void FillVector(T& Vector, float* Values)
{
    static_assert(IsVector<T>());
    for (int i=0; i<Vector.length(); i++)
    {
	Vector[i] = Values[i];
    }
}




template <typename T>
inline void FillMatrix(T& Matrix, float* Values)
{
    static_assert(IsMatrix<T>());
    int i = 0;
    for (int x=0; x<Matrix.length(); x++)
    {
	for (int y=0; y<Matrix.length(); y++)
	{
	    Matrix[x][y] = Values[i];
	    i++;
	}
    }
}




template <typename T>
inline string ReprVector(T& Vector)
{
    static_assert(IsVector<T>());
    std::ostringstream Repr;
    Repr << "<Vector: " << Vector[0];
    for (int i=1; i<Vector.length(); i++)
    {
	Repr << ", " << Vector[i];
    }
    Repr << ">";
    return Repr.str();
}




template <typename T>
inline string ReprMatrix(T& Matrix)
{
    static_assert(IsMatrix<T>());
    std::ostringstream Repr;
    Repr << "<Matrix: ";
    for (int x=0; x<Matrix.length(); x++)
    {
	for (int y=0; y<Matrix.length(); y++)
	{
	    if (!(x == 0 && y == 0)) Repr << ", ";
	    Repr << Matrix[x][y];
	}
    }
    Repr << ">";
    return Repr.str();
}




// TODO : find a cleaner way to do this.  `if constexpr` would be nice.
#define PERMUTE(T, Name) inline void FillThing(T& Name, float* Values) { Fill##Name<T>(Name, Values); }
PERMUTE(glm::vec2, Vector)
PERMUTE(glm::vec3, Vector)
PERMUTE(glm::vec4, Vector)
PERMUTE(glm::mat2, Matrix)
PERMUTE(glm::mat3, Matrix)
PERMUTE(glm::mat4, Matrix)
#undef PERMUTE
#define PERMUTE(T, Name) inline string ReprThing(T& Name) { return Repr##Name<T>(Name); }
PERMUTE(glm::vec2, Vector)
PERMUTE(glm::vec3, Vector)
PERMUTE(glm::vec4, Vector)
PERMUTE(glm::mat2, Matrix)
PERMUTE(glm::mat3, Matrix)
PERMUTE(glm::mat4, Matrix)
#undef PERMUTE




template <typename T>
PyObject* TypeObjectNew(PyTypeObject* Type, PyObject* Args, PyObject* Kwargs)
{
    MathHandle<T>* Self;
    // TODO : This is segfaulting in some situations:
    Self = (MathHandle<T>*)(Type->tp_alloc(Type, 0));
    return (PyObject*)Self;
}




// TODO : Returning -1 causes segfaults?  What is the correct error code?
template <typename T>
int TypeObjectInit(PyObject* Self, PyObject* Args, PyObject* Kwargs)
{
    MathHandle<T>* Handle = (MathHandle<T>*)Self;
    T& Vector = Handle->Wrapped;
    Vector = T();
    
    const int ArgCount = PyTuple_Size(Args);
    if (ArgCount == 0)
    {
	return 0;
    }

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
PyObject* TypeObjectPrint(PyObject* Self)
{
    MathHandle<T>* Handle = (MathHandle<T>*)Self;
    string Repr = ReprThing(Handle->Wrapped);
    return PyUnicode_FromString(Repr.data());
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
    TypeObject.tp_repr = TypeObjectPrint<T>;

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




PyObject* InitMatrixHelper(glm::mat4 Matrix)
{
    PyObject *Args = PyTuple_New(0);
    PyObject *Initialized = PyObject_CallObject((PyObject*)&Mat4Type, Args);
    MathHandle<glm::mat4>* Thing = (MathHandle<glm::mat4>*)Initialized;
    if (Thing)
    {
	Thing->Wrapped = Matrix;
    }
    Py_DECREF(Args);
    return Initialized;
}




PYTHON_API(PerspectiveMatrix)
{
    if (nargs == 4)
    {
	// fov aspect near far
	return InitMatrixHelper(
	    glm::perspective(
		(float)PyFloat_AsDouble(args[0]),
		(float)PyFloat_AsDouble(args[1]),
		(float)PyFloat_AsDouble(args[2]),
		(float)PyFloat_AsDouble(args[3])));
    }
    Py_RETURN_NONE;
}




PYTHON_API(OrthographicMatrix)
{
    if (nargs == 6)
    {
	// left right bottom top near far
	return InitMatrixHelper(
	    glm::ortho(
		(float)PyFloat_AsDouble(args[0]),
		(float)PyFloat_AsDouble(args[1]),
		(float)PyFloat_AsDouble(args[2]),
		(float)PyFloat_AsDouble(args[3]),
		(float)PyFloat_AsDouble(args[4]),
		(float)PyFloat_AsDouble(args[5])));
    }
    Py_RETURN_NONE;
}




PYTHON_API(LookAtMatrix)
{
    if (nargs == 9)
    {
	auto Eye = glm::vec3(
	    (float)PyFloat_AsDouble(args[0]),
	    (float)PyFloat_AsDouble(args[1]),
	    (float)PyFloat_AsDouble(args[2]));
	auto Center = glm::vec3(
	    (float)PyFloat_AsDouble(args[3]),
	    (float)PyFloat_AsDouble(args[4]),
	    (float)PyFloat_AsDouble(args[5]));
	auto Up = glm::vec3(
	    (float)PyFloat_AsDouble(args[6]),
	    (float)PyFloat_AsDouble(args[7]),
	    (float)PyFloat_AsDouble(args[8]));
	return InitMatrixHelper(glm::lookAt(Eye, Center, Up));
    }
    Py_RETURN_NONE;
}




PYTHON_API(RotationMatrix)
{
    if (nargs == 4)
    {
	glm::mat4 Identity;
	float Angle = (float)PyFloat_AsDouble(args[0]);
	glm::vec3 Pivot = glm::vec3(
	    (float)PyFloat_AsDouble(args[1]),
	    (float)PyFloat_AsDouble(args[2]),
	    (float)PyFloat_AsDouble(args[3]));
	return InitMatrixHelper(glm::rotate(Identity, Angle, Pivot));
    }
    Py_RETURN_NONE;
}




PYTHON_API(ScaleMatrix)
{
    if (nargs == 3)
    {
	glm::mat4 Identity;
	glm::vec3 Scale = glm::vec3((float)PyFloat_AsDouble(args[0]),
				    (float)PyFloat_AsDouble(args[1]),
				    (float)PyFloat_AsDouble(args[2]));
	 return InitMatrixHelper(glm::scale(Identity, Scale));
    }
    Py_RETURN_NONE;
}




PYTHON_API(TranslationMatrix)
{
    if (nargs == 3)
    {
	glm::mat4 Identity;
	glm::vec3 Coords = glm::vec3((float)PyFloat_AsDouble(args[0]),
				     (float)PyFloat_AsDouble(args[1]),
				     (float)PyFloat_AsDouble(args[2]));
	return InitMatrixHelper(glm::translate(Identity, Coords));
    }
    Py_RETURN_NONE;
}




// TODO : remove this and override the * operator instead?
PYTHON_API(MultiplyMatrices)
{
    if (nargs >= 2)
    {
	glm::mat4 Result(((MathHandle<glm::mat4>*)args[0])->Wrapped);
	for (int i=1; i<nargs; i++)
	{
	    Result = Result * ((MathHandle<glm::mat4>*)args[i])->Wrapped;
	}
	return InitMatrixHelper(Result);
    }
    Py_RETURN_NONE;
}
