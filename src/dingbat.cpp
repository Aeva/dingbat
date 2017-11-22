

#include <Python.h>
#include "object_handle.h"
#include "matrix_math.h"
#include "context.h"
#include "shaders.h"
#include "buffers.h"
#include "painter.h"




PyMethodDef ThroughputMethods[] = {
    CONTEXT_BINDINGS
    SHADER_BINDINGS
    BUFFER_BINDINGS
    PAINTER_BINDINGS
    MATH_BINDINGS
    {"create_test_handle", (PyCFunction)CreateTestHandle, METH_FASTCALL, NULL},
    {NULL, NULL, 0, NULL}
};




PyModuleDef ModuleDef = {
    PyModuleDef_HEAD_INIT,
    "dingbat",
    "",
    -1,
    ThroughputMethods
};




PyMODINIT_FUNC PyInit_dingbat(void)
{
    PyObject *Module = PyModule_Create(&ModuleDef);
    if (!InitObjectHandleType(Module))
    {
	return NULL;
    }
    if (!InitMathTypes(Module))
    {
	return NULL;
    }
    
    return Module;
}
